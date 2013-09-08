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
#include <stdio.h>
#include "params.h"
#include "../timing.h"

extern unsigned int width;
extern unsigned int height;
extern float x0;
extern float x1;
extern float y0;
extern float y1;
extern int maxIterations;
extern int *buf;
extern int NUMTHREAD;

static int mandel(float c_re, float c_im, int count) {
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i) {
        if (z_re * z_re + z_im * z_im > 4.f)
            break;

        float new_re = z_re*z_re - z_im*z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    return i;
}
/*
void mandelbrot_chunks(void *tp)
{
  for (int i = tp->my_start; i < tp->my_end; i++)
    for (int j = 0; j < max_col; j++) {
      tp->p[i][j] = mandel( complex(scale(i), scale(j)), depth);
    }
}
*/
void *print_threads(void* arg) 
{
		reset_and_start_timer();
	// TODO: Your code here
	// Thread Parameters
	thread_parameters* params = (thread_parameters*)arg;
	const int tid		= params->tid;
	const int width		= params->width;
	const int numRows	= params->rowsPerThread;
	const int myFirstRow= tid * numRows + params->extraRows;
	const int myLastRow = myFirstRow + numRows - 1;

	const float x0		= params->x0;
	const float x1 		= params->x1;
	const float y0 		= params->y0;
	const float y1 		= params->y1;
	const float dx = (x1 - x0) / width;
	const float dy = (y1 - y0) / numRows;
	// variables for mandelbrot calculation
	float x, y;
	int index;
	int * output = params->output;
#ifdef DEBUG
	printf("TID(%d):: rows [%d : %d], y=[%3.2f : %3.2f]\n",
		   	tid, myFirstRow, myLastRow, y0, y1);
	#ifdef VERBOSE
		int count=0, dIm=0, dIx=0;
		int minIndex=768*512, maxIndex=0;
	#endif
#endif
	for (int j = myFirstRow; j <= myLastRow; j++) {
		for (int i = 0; i < width; ++i) {
			x = x0 + i * dx;
			y = y0 + j * dy;

			index = (j * width + i);
			output[index] = mandel(x, y, maxIterations);
#ifdef VERBOSE
			if(index < minIndex){
				minIndex = index;
				dIm++;
			} else if(index > maxIndex) {
				maxIndex = index;
				dIx++;
			}
			count++;
			if(index%1000 == 0)
				printf("TID(%d):: output[%d] = %d, x=%3.3f, y=%3.3f\n", 
						tid, index, output[index], x, y);
#endif
		}
	}
#ifdef VEROSE
	printf("print_threads()::TID(%d) | count=%d, minIndex = %d, dIm=%d, maxIndex = %d, dIx=%d\n",
		   	tid, count, minIndex, dIm, maxIndex, dIx);
#endif
	double dt = get_elapsed_mcycles();
	printf("[mandelbrot thread (%d)]:\t[%.3f] millon cycles\n", tid, dt);

	return (void*) params;
}
