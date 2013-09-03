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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#define DEBUG 1
#define SERIAL_FILL 1
/* use serial fill for now because the parallel fill is not working */

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

/* Maximum number of threads */
const int g_MAX_PROCESSOR_THREADS = 2;
/* default number of processor threads & its mutex
   10 by default unless a number is passed in as a cmd line arg */
int g_NUM_PROCESSOR_THREADS = 4;

/* thread function */
void * doMMult_thread(void * params);
void * fillMatrices_thread( void * arg);
void fillMatrices_serial(double* A, double* B, double* C, int d);

int main(int argc, char *argv[])
{
	int d, i, j, k, nRows;
	int code;	/* pthread status code */
	double *A, *B, *C;
	struct timeval begin, end;
	pthread_t *tids;
	thread_parameters * data;

	if (argc == 2)
		dimension = atoi(argv[1]);

	nRows = dimension / g_NUM_PROCESSOR_THREADS;
	printf("Dimension = %d\n", dimension);
	printf("nRows per thread = %d\n", nRows);

	/* allocate threadIDs for each of the threads */
	tids = (pthread_t*) malloc(g_NUM_PROCESSOR_THREADS * sizeof(pthread_t));

	/* seed for pseudorandom number generation */
	srand(292);
	/* TODO: why is this loop here?? it only executes once */
	for (d = dimension ; d < dimension+1; d=d+128 ) {
		int runs = 0;
		for ( ; runs>=0; runs--) {
			/* print run number */
			printf("\n\nRun ## r%d\n", runs);
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
				data->last = data->first + nRows -1;
				data->A = A;
				data->B = B;
				data->C = C;
#ifdef DEBUG
				printf("\nmain(), pre: tid #%d, first=%d, last=%d, C[%d] = %0.3f\n", i, data->first, data->last, data->first, C[data->first]);
#endif
				/* spawn threads */
				code = pthread_create(&tids[i], NULL, doMMult_thread, (void*) data);
#ifdef DEBUG
				if( code != 0 ){
					printf( "\nmain(): Error: unable to create processor thread #%d\n",i );
				}else{
					printf("\nmain(): Calc Thread #%d created successfully!\n", i);
				}
#endif	
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

void * doMMult_thread( void * arg){
	/* local variables */
	thread_parameters* params = (thread_parameters*)arg;

	int intTID = params->tid;
	int d, r, c, k, firstRow, lastRow, count=0;
	int cix, aix, bix;
	d = params->dim;
	firstRow = params->first;
	lastRow = params->last;

#ifdef DEBUG
	printf("doMMult_Thread():: tid=%d, d=%d, firstRow=%d, lastRow=%d, A[%d]=%0.3f\n", intTID, d, firstRow, lastRow, firstRow, params->A[firstRow]);
#endif
	for(r = firstRow; r <= lastRow; r++) {
		for(c = 0; c < d; c++) {
			for(k = 0; k < d; k++) {
				aix = d*r+k;
				bix = d*k+c; 
				cix = d*r+c;
				params->C[cix] += params->A[aix] * params->B[bix]; 
#ifdef DEBUG
				count++;
				printf("tid=%d, r=%d, c=%d, cix=%d, aix=%d, bix=%d\n", intTID, r, c, cix, aix, bix); 
#endif
			}
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
