/**********************************************************
 * FILENAME:    MatMultPar.c
 * DESCRIPTION: C-source for matrix multiply - parallel 
 * AUTHOR:      James Matthew Welch [JMW]
 * SCHOOL:      Arizona State University
 * CLASS:       CSE520: Computer Architecture II
 * INSTRUCTOR:  Dr. Carole-Jean Wu
 * SECTION:     83380
 * TERM:        Fall 2013
 *********************************************************/
/* Points to consider in the design of the parallel algorithm: 
 * (1) assume cache line size of 64 bytes == 8 * doubles (8-bytes each)  
 *		--> load 8 doubles at a time to take advantage of the cache
 * (2) attempt to avoid cache thrashing by loading blocks of data into the
 * cache whenever possible
 * */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
/* #define DEBUG 1 */
/* use serial fill for now because the parallel fill is not working */
#define SERIAL_FILL 1
/* block the inner loop to maximize locality */
#define LOOP_BLOCKING 0
/* fill with random numbers instead of linearly increasing data */
#define RAND_FILL 1 
/* accumulate C in a local variable */
#define LOCAL_ACC 1  
/* change d size as needed */

int dimension = 840; /* LCM of {8,7,6,5,4,3,2} - always divides evenly mod would be better*/

/* structure to pass parameters to a processor thread */
typedef struct _thread_parameters{
	int tid;
	double* A;
	double* B;
	double* C;
	int     dim;
	int		first;
	int		last;
} thread_parameters;

/* default number of processor threads */
int g_NUM_PROCESSOR_THREADS = 4;

/* thread function */
void * doMMult_thread(void * params);
void * fillMatrices_thread( void * arg);
void fillMatrices_serial(double* A, double* B, double* C, int d);

int main(int argc, char *argv[])
{
	int d, i, j, k, nRows, extraRows, firstRow, aix, bix, cix;
#ifdef VERBOSE
	int count;
#endif
	int code;	/* pthread status code */
	double *A, *B, *C;
	struct timeval begin, end;
	pthread_t *tids;
	thread_parameters * data;
	int runs = 3;

	if (argc > 1)
		dimension = atoi(argv[1]);
	if (argc > 2){ 
		g_NUM_PROCESSOR_THREADS = atoi(argv[2]);
		if(g_NUM_PROCESSOR_THREADS > dimension){
			printf("ERROR: More threads (%d) requested than matrix dimension (%d). Using %d threads.\n", g_NUM_PROCESSOR_THREADS, dimension, dimension);
			g_NUM_PROCESSOR_THREADS = dimension;
		}
	}
	if (argc > 3)
		runs = atoi(argv[3]);

	nRows = dimension / g_NUM_PROCESSOR_THREADS;
	extraRows = dimension % g_NUM_PROCESSOR_THREADS;
	printf("Dimension = %d\n", dimension);
	printf("Number of Threads = %d\n", g_NUM_PROCESSOR_THREADS);
	printf("nRows per thread = %d\n", nRows);
	printf("extraRows = %d\n", extraRows);



	/* allocate threadIDs for each of the threads */
	tids = (pthread_t*) malloc(g_NUM_PROCESSOR_THREADS * sizeof(pthread_t));

	/* seed for pseudorandom number generation so we generate the same numbers*/
	srand(292);
	/* This loop is useful for testing many sizes of matrices */
	for (d = dimension ; d < dimension+1; d=d+128 ) {
		for ( ; runs>=0; runs--) {
#ifdef DEBUG
			/* print run number */
			printf("\n\nRun ## r%d\n", runs);
#endif
			/* allocate square arrays */
			A = (double*)malloc(d*d*sizeof(double));
			B = (double*)malloc(d*d*sizeof(double));
			C = (double*)malloc(d*d*sizeof(double));
			gettimeofday(&begin, NULL); /* returns the wall clock time */
			/* fill matrices in serial or parallel fashion */
#if SERIAL_FILL
			/* fill the matrices sequentially */
#ifdef DEBUG
			printf("Filling in SERIAL: d = %d\n", d);
#endif
			
			fillMatrices_serial(A, B, C, d);
#ifdef DEBUG
			printf("FILLED: d = %d\n", d);
#endif

#else /* Fill in Parallel */
			/* fill matrices with threads */
#ifdef DEBUG
			printf("Filling in PARALLEL: NumThreads=%d\n", g_NUM_PROCESSOR_THREADS);
#endif
			for (i = 0; i < g_NUM_PROCESSOR_THREADS; ++i) {
				/* allocate thread params for each of the threads */
				data = malloc(sizeof(thread_parameters));
				data->tid = i;
				data->dim = dimension;
				data->first = nRows*i;
				data->last = data->first + nRows -1;
				/* TODO: need to account for rows that are remainder */
				data->A = A;
				data->B = B;
				data->C = C;

				/* spawn threads */
				code = pthread_create(&tids[i], NULL, fillMatrices_thread, (void*) data);
				if( code != 0 ){
					printf( "\nmain(): Error: unable to create processor thread #%d\n",i );
				}else{
#ifdef DEBUG
					printf("\nmain(): Fill Thread #%d created successfully!\n", i);
#endif	
				}
			}
		
			/* join threads */
			for( i = 0; i < g_NUM_PROCESSOR_THREADS; ++i)
				pthread_join( tids[ i ], NULL );
#endif /* end SERIAL_FILL */
			gettimeofday(&end, NULL);
			printf("[%d/%d] init loop took %ldus\t", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));

			gettimeofday(&begin, NULL);

			for (i = 0; i < g_NUM_PROCESSOR_THREADS; ++i) {
				/* allocate thread params for each of the threads */
				data = malloc(sizeof(thread_parameters));
				data->tid = i;
				data->dim = dimension;
				data->first = nRows*i;
				data->last = data->first + nRows - 1;
				data->A = A;
				data->B = B;
				data->C = C;
#ifdef DEBUG
				printf("\nmain(), pre: tid #%d, first=%d, last=%d, C[%d] = %0.3f\n", i, data->first, data->last, data->first, C[data->first]);
#endif
				/* spawn threads */
				code = pthread_create(&tids[i], NULL, doMMult_thread, (void*) data);
				if( code != 0 ){
					printf( "\nmain(): Error: unable to create processor thread #%d\n",i );
				}else{
#ifdef DEBUG
					printf("\nmain(): Calc Thread #%d created successfully!\n", i);
#endif	
				}
			}

			/* main thread cleans up by calculating the extra rows while
			 * threads are working */
#ifdef DEBUG
			printf("i = %d, dim = %d, nRows = %d, i*nRows=%d, extra=%d\n", i, dimension, nRows, i*nRows, extraRows);
			printf("main_first=%d, last=%d\n", i*nRows, i*nRows + extraRows -1);
#endif
			firstRow = nRows*g_NUM_PROCESSOR_THREADS;
			for(i = firstRow; i < d; i++) {
				for(j = 0; j < d; j++) {
					for(k = 0; k < d; k++) {
						aix = d*i+k;
						bix = d*k+j;
						cix = d*i+j;
						C[cix] += A[aix] * B[bix];
#ifdef VERBOSE
						count++;
						printf("r=%d, c=%d, cix=%d, aix=%d, bix=%d\n", i, j, cix, aix, bix); 
#endif
					}
				}
			}


			/* join threads */
			for( i = 0; i < g_NUM_PROCESSOR_THREADS; ++i)
				pthread_join( tids[ i ], NULL );


#ifdef DEBUG
			printf("main():: computation threads joined\n");
			for (i = 0; i < d*d; i+=d) {
				printf("main():: C[%d]=%0.3f\n", i, C[i]);
			}
#endif
			gettimeofday(&end, NULL);
			printf("[%d/%d] %ldus\n", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));

			free(A);
			free(B);
			free(C);
		}
		printf("\n");
	}
	free(tids);

	return 0;
}
/* ////////////////////////////////////////////////////////////////////// */
void * doMMult_thread( void * arg){
	/* local variables */
	thread_parameters* params = (thread_parameters*)arg;

#ifdef DEBUG
	int intTID = params->tid; 
	int count=0;
#endif
	int d, r, c, k, firstRow, lastRow;
	double acc; /* local accumulator variable to prevent multiple writes to C */
	int cix, aix, bix;
	d = params->dim;
	firstRow = params->first;
	lastRow = params->last;
#if LOOP_BLOCKING
	double * B_column;
	B_column = (double*)malloc(d * sizeof(double));
#endif

#ifdef DEBUG
	printf("doMMult_Thread():: tid=%d, d=%d, firstRow=%d, lastRow=%d, A[%d]=%0.3f\n", intTID, d, firstRow, lastRow, firstRow, params->A[firstRow]);
#endif
	for(r = firstRow; r <= lastRow; r++) {
		for(c = 0; c < d; c++) {
			/* preload the column of B into a local variable to reduce cache
			 * thrashing and contention - hopefully the stride prefetcher
			 * will speed this up*/
#if LOOP_BLOCKING
			for(k=0; k<d; k++){
				bix=d*k+c;
				B_column[k] = params->B[bix];
			}
#endif
			acc = 0;
			cix = d*r+c;
			for(k = 0; k < d; k++) {
				aix = d*r+k;
				bix = d*k+c; 
#if LOOP_BLOCKING
				params->C[cix] += params->A[aix] * B_column[k]; 
#else
				/* accumulate C[cix] into a local variable to prevent
				 * multiple writes to memory */
#if LOCAL_ACC
				acc += params->A[aix] * params->B[bix]; 
#else
				/* TODO: "block" the loop */
				params->C[cix] += params->A[aix] * params->B[bix]; 
#endif /* LOCAL_ACC */
#endif
#ifdef VERBOSE
				count++;
				printf("tid=%d, r=%d, c=%d, cix=%d, aix=%d, bix=%d\n", 
						intTID, r,     c,    cix,    aix,   bix); 
#endif
			}
#if LOCAL_ACC
			params->C[cix] = acc;
#endif
		}
	}
#ifdef DEBUG
	printf("doMMult_Thread()::POST:: thread %d, C[%d]=%0.3f, count=%d\n", 
			intTID, firstRow, params->A[firstRow], count);
#endif
	return NULL;
}

void * fillMatrices_thread( void * arg){
	/* function fills the arrays */
	thread_parameters* params = (thread_parameters*)arg;

	int intTID = params->tid;
	int d, i, j, init, final;
	d = params->dim;
	init = params->first;
	final = params->last;

	printf("I'm thread %d, I have %d rows [%d...%d]\n", intTID, d, init, final);

	for(i = init; i <= final; i++) {
		for(j = init; j <= final; j++) {
			params->A[d*i+j] = (rand()/(RAND_MAX - 1.0));
			params->B[d*i+j] = (rand()/(RAND_MAX - 1.0));
			params->C[d*i+j] = 0.0;
		}
	}
#ifdef DEBUG
	printf("fillMatrices_thread():: I'm thread %d, A[%d] = %f\n", intTID, init, params->A[init]);
#endif
	return NULL;
}

void fillMatrices_serial(double* A, double* B, double* C, int d){
	int i, j;
#if RAND_FILL
	for(i = 0; i < d; i++) {
		for(j = 0; j < d; j++) {
			A[d*i+j] = (rand()/(RAND_MAX - 1.0));
			B[d*i+j] = (rand()/(RAND_MAX - 1.0));
			C[d*i+j] = 0.0;
		}
	}
#else /* fill matrices with monotonically increasing data */
	for(i = 0; i < d; i++) {
		for(j = 0; j < d; j++) {
			A[d*i+j] = d*i+j;
			B[d*i+j] = d*i+j;
			C[d*i+j] = 0.0;
		}
	}
#endif
}
