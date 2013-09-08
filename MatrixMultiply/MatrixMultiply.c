/**********************************************************
 * FILENAME:    MatrixMultiply.c
 * DESCRIPTION: C-source for matrix multiply - serial 
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

/* change d size as needed */
int dimension = 840;

int main(int argc, char *argv[])
{
	int d, i, j, k, aix, bix, cix, count=0;
	double *A, *B, *C;
	struct timeval begin, end;

	int runs = 3;
	if (argc == 2)
		dimension = atoi(argv[1]);
	if (argc == 3)
		runs = atoi(argv[2]);

	printf("Dimension = %d\n", dimension);
	printf("Serial Matrix Multiplication, executing %d runs\n", runs+1);

	srand(292);
	for (d = dimension ; d < dimension+1; d=d+128 ) {
		for ( ; runs>=0; runs--) {
			A = (double*)malloc(d*d*sizeof(double));
			B = (double*)malloc(d*d*sizeof(double));
			C = (double*)malloc(d*d*sizeof(double));
			gettimeofday(&begin, NULL); /* returns the wall clock time */


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

			gettimeofday(&end, NULL);
			printf("[%d/%d] init loop took %ldus\t", d, 5-runs, ((end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec)));
			
			gettimeofday(&begin, NULL);
			for(i = 0; i < d; i++) {
				for(j = 0; j < d; j++) {
					for(k = 0; k < d; k++) {
						aix = d*i+k;
						bix = d*k+j;
						cix = d*i+j;
						C[cix] += A[aix] * B[bix];
#ifdef DEBUG
						count++;
						printf("r=%d, c=%d, cix=%d, aix=%d, bix=%d\n", i, j, cix, aix, bix); 
#endif
					}
				}
			}
#ifdef DEBUG
			printf("\nmain():: serial computation complete, count=%d\n", count);
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

	return 0;
}
