#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <unistd.h>
#include <time.h>

#define N 5

thread_t tid, thr[N];

int NW[N][N];

//Mutex for each city's array
mutex_t lock[N];
struct timespec Tim={0,10000};

//Struct to hold each city's data
typedef struct dv
{	int sender;
	unsigned int link[N];
	struct dv* next;
}	node, *node_ptr;

node_ptr Q_head[N], Q_tail[N];


void *city(void *arg)
{
	int myId = (int) arg;
	
	int k, j, m;
	int DV[N];
}

int main(int argc, char* argv[])
{
	//If we don't have the appropriate # of arguments, exit.
	if (argc != 2) {
		printf("Usage is:\tSTAGE%d filename\n",2);
		return 1;
	}
	
	int i, status;
	FILE *fp;
	
	//Open the file input by user
	fp=fopen(argv[1],"r");
	
	//Initiate the head, tail and mutex needed by each thread
	for (i = 0; i < N; i++) {
		mutex_init(&lock[i],USYNC_THREAD,NULL);
		Q_head[i] = NULL;
		Q_tail[i] = NULL;
	}
		
	//Initialize all of the threads needed for the program
	for (i = 0; i < N; i++)
		thr_create(NULL,0,city,(void*)i, THR_BOUND, &thr[i]);
	
	//Wait for all the threads to finish
	while(thr_join(0,&tid, (void**)&status)==0)
		printf("Main - status=%d, first_id=%d\n",status,tid);
	
	//Destroy the mutex
	for (i = 0; i < N; i++) mutex_destroy(&lock[i]);
	
	//Close the file
	fclose(fp);
	
	return 0;
}
