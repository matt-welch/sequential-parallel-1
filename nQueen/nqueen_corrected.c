#include <assert.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_THREADS 2
#define Q_CAPACITY 1000

/************ SERIAL APPROACH ************/
/**** column, left_diag and right_diag are bitmaps that indicate which positions are blocked ****/
int nq_serial(unsigned int col, unsigned int left, unsigned int right, int row, int n) 
{
  if (row == n) { 
    return 1;
  } else {
    int count = 0;
    unsigned int block = col | left | right;
    while (block + 1 < (1 << n)) {
      unsigned int open = (block + 1) & (~block);
      count += nq_serial(col | open, 
	      ((left | open) << 1) & ((1 << n) - 1), 
	      (right | open) >> 1, 
	      row + 1, 
	      n);
      block |= open;
    }
    return count;
  }
}


/*********** THREADS **************/

void * nq_parallel_thread(void * arg);

typedef struct nqueen_arg {
	//TODO: Add components that are placeholder for the following
	// 1. Thread ID
	// 2. Arguments for the thread function nq_parallel_thread()
	// 3. Return value (why do we need place holder for return value ?) 
} nqueen_arg, * nqueen_arg_t;


void * nq_parallel_thread(void * arg) {
  // TODO: Thread body
  return;
}


/**** Main procedure for threads approach ****/
int nq_parallel(unsigned int col, unsigned int left, unsigned int right, 
	int row, int n,
	int parallel_depth) 
{
	if (row == n) { 
    	return 1;
	} else {
		int n_threads = 0;
	    nqueen_arg_t args = 0;
	    unsigned int block = col | left | right;
	    int count = 0;
	    while (block + 1 < (1 << n)) {
			unsigned int open = (block + 1) & (~block);
			if (row < parallel_depth) {
				/****** spawn thread up to parallel depth ******/
				assert(n_threads < n);
				if (args == 0) {
					args = (nqueen_arg_t)malloc(sizeof(nqueen_arg) * n);
				}
				// TODO: package argument for the thread
				// TODO: spawn thread
				n_threads++;
			} 
			else {
				// TODO: Beyond the parallel depth use sequential approach
			}
	
			/****** wait for termination of spawned threads ******/
			int i;
			for (i = 0; i < n_threads; i++) {
				// TODO : Ensure thread termination
				// Do NOT forget to collect return values from each thread :)
			}
		}
    	return count;
	}
}

/****** END of THREADS ******/


/****** THREADS WITH WORK QUEUE ******/

/* data structure representing a queue of work to be done
   (a unit of work is represented by args to nqueen procedures) */
typedef struct work_queue {
  pthread_mutex_t mx;
  int head;
  int size;
  int capacity;
  nqueen_arg_t args;
} work_queue, *work_queue_t;


void work_queue_add(work_queue_t wq, unsigned int col, unsigned int left, unsigned int right, int row, int n, int parallel_depth) {
	int sz = wq->size;
	int capacity = wq->capacity;
	nqueen_arg_t args = wq->args;

	if (sz >= capacity) {
		int new_capacity = capacity * 2;
		if (new_capacity <= capacity) new_capacity = capacity + 1;
		nqueen_arg_t new_args = (nqueen_arg_t)malloc(sizeof(nqueen_arg) * new_capacity);
		// Copy all arguments of existing queue to the new queue with updated capacity
		bcopy(args, new_args, sizeof(nqueen_arg) * capacity);
		wq->capacity = capacity = new_capacity;
		wq->args = args = new_args;
	}
	assert(sz < capacity);
	nqueen_arg_t arg = args + sz;
	// TODO : Package the right arg components
	wq->size = sz + 1;
}

work_queue_t mk_work_queue() {
	int capacity = Q_CAPACITY;
	work_queue_t wq = (work_queue_t)malloc(sizeof(work_queue));
	nqueen_arg_t args = (nqueen_arg_t)malloc(sizeof(nqueen_arg) * capacity);
	assert(wq);
	assert(args);
	// Initialize the mutex
	pthread_mutex_init(&wq->mx, 0);
	wq->head = 0;
	wq->size = 0;
	wq->capacity = capacity;
	wq->args = args;

	return wq;
}

int nq_work(work_queue_t wq,
	unsigned int col, unsigned int left, unsigned int right, 
	int row, int n,
	int parallel_depth) 
{
  if (row == n) { 
    return 1;
  } else if (row == parallel_depth) {
    /* return immediately after generating work at this depth */
    work_queue_add(wq, col, left, right, row, n, -1);
    return 0;
  } else {
    int count = 0;
	// TODO: Serial recursive approach
	// Call nq_work recursively with appropriate arguments
    return count;
  }
}

void * nq_worker(void * wq_) {
  work_queue_t wq = (work_queue_t)wq_;
  while (1) {
    nqueen_arg_t arg = 0;
    /* try to fetch work */
	// TODO : Lock Mutex
	// CRITICAL SECTION : get the head of work queue and retreive "arg". Then, update head of queue
	// TODO : Unlock Mutex

    if (arg ==0) break;		/* no work left, quit */
    // TODO: Call nq_work recursively with appropriate arguments
  }
}


/**** Master thread that spawns the worker threads and collects the results ****/
int nq_master(int n, int parallel_depth) {
  work_queue_t wq = mk_work_queue();
  /* generate work */
  int count = nq_work(wq, 0, 0, 0, 0, n, parallel_depth);
  int i;
  /* create pthreads that do the generated work */

  pthread_t * thids = (pthread_t *)malloc(sizeof(pthread_t) * N_THREADS);
  for (i = 0; i < N_THREADS; i++) {
    // TODO: Create Worker Threads (thread function nq_worker())
  }
  for (i = 0; i < N_THREADS; i++) {
    // TODO : Endure Thread termination
  }
  for (i = 0; i < wq->size; i++) {
    // TODO : Collect Thread return values from wq
  }
  return count;
}


void usage(){
	printf ("************* USAGE ***********\n\n");
	printf ("\t ./nqueen <APPROACH> <N> [DEPTH]\n");
	printf ("\t APPROACH = 0 => Serial\n");
	printf ("\t APPROACH = 1 => Threads\n");
	printf ("\t APPROACH = 2 => Threads with Work Pool\n");
	printf ("\t N = Number of queens\n");
	printf ("\t DEPTH = Recursion depth for Threads and Threads with Work approach\n");
	return;
}

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, 0);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main(int argc, char * argv[]) {

	if (argc < 3 || argc > 4){
		usage();
		return -1;
	}

	int st = atoi(argv[1]);
	int n = atoi(argv[2]);
	int parallel_depth = -1;

  	if (argc > 3) {
		parallel_depth = atoi(argv[3]);
	}

	double t0 = cur_time();
	int n_solutions = 0;
	switch (st){
	case 0:
		printf ("\n\n****** Solving nQueen using serial recursion ******\n\n");
		n_solutions = nq_serial (0, 0, 0, 0, n);
		break;
	case 1:
		printf ("\n\n****** Solving nQueen using threads ******\n\n");
		n_solutions = nq_parallel (0, 0, 0, 0, n, parallel_depth);
		break;
	case 2:
		printf ("\n\n****** Solving nQueen using work pool and threads ******\n\n");
		n_solutions = nq_master (n, parallel_depth);
		break;
	default:
		printf ("\n\n****** No Strategy defined for %d ******\n\n", st);
		return -1;
	}
	double t1 = cur_time();

	printf("%.3f sec (%d queen = %d)\n\n", t1 - t0, n, n_solutions);

	return 0;
}

