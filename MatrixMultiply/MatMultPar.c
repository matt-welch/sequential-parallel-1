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
/* change d size as needed */
int dimension = 840; /* LCM of {8,7,6,5,4,3,2} - always divides evenly mod would be better*/

// structure to pass parameters to a processor thread
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
const int g_NUM_PROCESSOR_THREADS = 2;

/* thread function */
void * doMMult_thread(void * params);

int main(int argc, char *argv[])
{
	int d, i, j, k, nRows;
	int code;	/* pthread status code */
	double *A, *B, *C;
	struct timeval begin, end;

	if (argc == 2)
		dimension = atoi(argv[1]);

	nRows = dimension / g_NUM_PROCESSOR_THREADS;
	printf("Dimension = %d\n", dimension);
	printf("nRows per thread = %d\n", nRows);

	/* allocate threadIDs for each of the threads */
	pthread_t *tids;
	thread_parameters * data;
	tids = (pthread_t*) malloc(g_NUM_PROCESSOR_THREADS * sizeof(pthread_t));


	/* seed for pseudorandom number generation */
	srand(292);
	/* TODO: why is this loop here?? it only executes once */
	for (d = dimension ; d < dimension+1; d=d+128 ) {
		int runs = 3;
		for ( ; runs>=0; runs--) {
			/* print run number */
			printf("\n\nRun ## r%d\n", runs);
			/* allocate square arrays */
			A = (double*)malloc(d*d*sizeof(double));
			B = (double*)malloc(d*d*sizeof(double));
			C = (double*)malloc(d*d*sizeof(double));
			gettimeofday(&begin, NULL); /* returns the wall clock time */


			for(i = 0; i < d; i++) {
				for(j = 0; j < d; j++) {
					A[d*i+j] = (rand()/(RAND_MAX - 1.0));
					B[d*i+j] = (rand()/(RAND_MAX - 1.0));
					C[d*i+j] = 0.0;
				}
			}

			gettimeofday(&end, NULL);
			printf("[%d/%d] init loop took %ldus\t", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));

			gettimeofday(&begin, NULL);

			for (i = 0; i < g_NUM_PROCESSOR_THREADS; ++i) {
				/* determine the number of rows/columns to assign to the thread */

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
				code = pthread_create(&tids[i], NULL, doMMult_thread, (void*) data);
#ifdef DEBUG
				if( code != 0 ){
					printf( "\nmain(): Error: unable to create processor thread #%d\n",i );
				}else{
					printf("\nmain(): Thread #%d created successfully!\n", i);
				}
#endif	

			}

			/* join threads */
			for( i = 0; i < g_NUM_PROCESSOR_THREADS; ++i)
				pthread_join( tids[ i ], NULL );

			free(data);

#ifdef DEBUG
			printf("main() :: Processor threads joined\n");
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
	double* A = params->A;
	double* B = params->B;
	double* C = params->C;

	int intTID = params->tid;
	int d, i, j, k, init, final;
	d = params->dim;
	init = params->first;
	final = params->last;

	printf("I'm thread %d, I have %d rows [%d...%d]\n", intTID, d, init, final);

#ifdef FOO
	for(i = 0; i < d; i++) {
		for(j = 0; j < d; j++) {
			for(k = 0; k < d; k++) {
				C[d*i+j] += A[d*i+k] * B[d*k+j]; /* TODO: why use += ?? */
			}
		}
	}
#endif
	return;
}

