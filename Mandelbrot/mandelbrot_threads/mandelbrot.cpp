/*
   Copyright (c) 2010-2011, Intel Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
met:

 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 * Neither the name of Intel Corporation nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.


 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#pragma warning (disable: 4244)
#pragma warning (disable: 4305)
#endif

#include <pthread.h>
#include <stdio.h>
#include <algorithm>
#include "../timing.h"
#include "mandelbrot_ispc.h"
#include "params.h"
using namespace ispc;

/* TODO: these can all be const since they do not change */
unsigned int width;
unsigned int height;
float x0;
float x1;
float y0;
float y1;
int maxIterations;
int *buf;
int NUMTHREAD = 8;
int numRuns = 5;

extern void mandelbrot_serial(float x0, float y0, float x1, float y1,
		int width, int height, int maxIterations,
		int output[]);

extern void mandelbrot_threads(float x0, float y0, float x1, float y1,
		int width, int height, int maxIterations,
		int output[]);

extern void *print_threads(void* params);

/* Write a PPM image file with the image of the Mandelbrot set */
static void
writePPM(int *buf, int width, int height, const char *fn) {
	FILE *fp = fopen(fn, "wb");
	fprintf(fp, "P6\n");
	fprintf(fp, "%d %d\n", width, height);
	fprintf(fp, "255\n");
	for (int i = 0; i < width*height; ++i) {
		// Map the iteration count to colors by just alternating between
		// two greys.
		char c = (buf[i] & 0x1) ? 240 : 20;
		for (int j = 0; j < 3; ++j)
			fputc(c, fp);
	}
	fclose(fp);
	printf("Wrote image file %s\n", fn);
}


int main() {

	width = 768;
	height = 512;
	x0 = -2;
	x1 = 1;
	y0 = -1;
	y1 = 1;
	maxIterations = 256;
	buf = new int[width*height]; 
	int rc;
	int rowsPerThread = height / NUMTHREAD; // TODO: not always evenly divisible by NUMTHREAD
	int extraRows = height % NUMTHREAD; // add these rows to main's job and offset others
#ifdef DEBUG
	 printf("main()::INIT:: nThreads = %d, rowsPerThread=%d, extraRows=%d\n",
			NUMTHREAD, rowsPerThread, extraRows);
#endif
 	// 
	// Run the serial implementation 3 times, reporting the minimum time.
	//
	double minSerial = 1e30;
	for (int i = 0; i < numRuns; ++i) {
		reset_and_start_timer();
		mandelbrot_serial(x0, y0, x1, y1, width, height, maxIterations, buf);
		double dt = get_elapsed_mcycles();
		minSerial = std::min(minSerial, dt);
	}

	printf("[mandelbrot serial]:\t\t[%.3f] millon cycles\n", minSerial);
	writePPM(buf, width, height, "mandelbrot-serial.ppm");

	/* Start of pthread version */
	// Clear out the  
	for (unsigned int i = 0; i < width * height; ++i)
		buf[i] = 0;
	//
	// And run the pthread implementation 3 times, again reporting the 
	// minimum time 
	//
	double minThread = 1e30;
	float DY = y1-y0;		// total range of Y
	float dy = DY / NUMTHREAD; // range of y per thread
	float main_y0 = y0; //TODO: need to adjust to account for remainder rows
	float main_y1 = y0 + dy;

	for (int i = 0; i < numRuns; ++i) {
		pthread_t threads[NUMTHREAD-1];
		reset_and_start_timer();
		
		// Loop to spawn NUMTHREAD-1 threads
		for (int k = 0; k < NUMTHREAD-1; k++) {
			// Spawn threads with appropriate argument setting what 
			// partial problem the thread will be working on
			thread_parameters *params = new thread_parameters;
			params->tid = k+1; // offset thread ID's to account for main() being thread 0
			params->x0 = x0;
			params->x1 = x1;
			/* TODO: TA: the following two variables don't make sense.  
			 * Why should each of the threads get the same range for y??????? */
			params->y0 = main_y0; //(params->tid-1)  * dy + y0;
			params->y1 = main_y1; //params->y0 + dy;

			params->rowsPerThread = rowsPerThread; 
			params->extraRows = extraRows;
			//TODO: needs to be adjusted for main thread to pick up the extra rows that are remainder
			params->width = width;
			params->maxIterations = maxIterations;
			params->output = buf;

	
			// HINT: define a struct to pass to print_threads instead of k
			rc = pthread_create(&threads[k], NULL, print_threads, (void *)params);
			if (rc){
				printf("ERROR: Thread Creation FAILED: %d\n", rc);
				exit(-1);
			}else{
#ifdef VERBOSE
				printf("main():: Thread(%d) spawned successfully\n", k);
#endif
			}

		}
		
		// TODO: Call mandelbrot_serial with the appropriate arguments
		// main() thread will also perfrom useful partial computation		
#ifdef DEBUG
		printf("main():: Working on rows [0 : %d], y=[%3.2f : %3.2f]\n", 
				rowsPerThread-1+extraRows, main_y0, main_y1);
#endif
		mandelbrot_serial(x0, main_y0, x1, main_y1, width, 
				rowsPerThread+extraRows, maxIterations, buf);

		// Guarantee target thread(s) termination 
		for (int k = 0; k < NUMTHREAD-1; k++){
			rc = pthread_join(threads[k], NULL/*&att*/);
			if (rc){
				printf("ERROR: Thread Join FAILED: %d\n", rc);
				exit(-1);
			}
		}
		double dt = get_elapsed_mcycles();
		minThread = std::min(minThread, dt);
	}

	printf("[mandelbrot thread]:\t\t[%.3f] millon cycles\n", minThread);
	writePPM(buf, width, height, "mandelbrot-thread.ppm");

	/* End of pthread version */
	
	// Report speedup over serial mandelbrot
	printf("\t\t\t\t(%.2fx speedup from threading)\n", minSerial/minThread);

	return 0;
}
