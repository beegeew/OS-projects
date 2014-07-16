/*
CS4352
Project 2 - Beverly G Wertz
STAGE4
Calculating an optimal path cost via threads representing links between cities
*/

#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <unistd.h>
#include <time.h>

#define N 5

//Setup our thread id's
thread_t tid, thr[N];

//Setup our overall distance vectors and # of times to loop
int NW[N][N], M = 10, loop_count = 0, changed[N];

//Mutex for each city's array
mutex_t lock[N];
struct timespec Tim={1,10000000};

//Struct to hold each city's data
typedef struct dv
{	int sender;
	unsigned int link[N];
	struct dv* next;
}	node, *node_ptr;

//Setup head and tail of queues for each 
node_ptr Q_head[N], Q_tail[N];

//headers for functions
void sendDV(int, int);
int processQ(int);

//City function executed by new threads
void *city(void *arg)
{
	//city ID
	int myId = (int) arg;
	
	//Setup values and Distance/connectivity arrays
	int k, j, m, change;
	unsigned int DV[N],CN[N];
	
	//lock the queue, then set the value and check the connectivity
	for (j = 0; j < N; j++)
	{
		mutex_lock(&lock[myId]);
		DV[j] = NW[myId][j];
		if (DV[j] < 10000) CN[j] = 1;
		else CN[j] = 0;
		
		if (j == myId)
			NW[myId][j] = DV[j] = CN[j] = 0;
		mutex_unlock(&lock[myId]);
	}
	//Printout the neighbors then the initial DV
	printf("Neighbors of city=%d are: ",myId);
	for (j = 0; j < N; j++) if (CN[j] == 1) printf("%d, ",j);
	printf("\nInitial DV of city=%d;  ",myId);
		
	for (k = 0; k < N; k++) {
		printf("(%d %d)  ",k,DV[k]);
	}
	printf("\n\n");
	
	//If connected, send the DV to the other processes, sleep, then process our own Q
	//if things change, reset the loop counter (global var in all processes) and start
	//loop over, once no changes occur in any thread for 10 cycles, exit.
	loop_count = 0;
	while( loop_count <=M) {
		for (j = 0; j < N; j++)
			if (j != myId && CN[j] == 1) sendDV(myId, j);
		printf("\n");
	
		nanosleep(&Tim, NULL);
		change = processQ(myId);
		if (change) {
			loop_count = 0;
			printf("DV of %d changed:  ",myId);
			for (j = 0; j < N; j++)
				printf("(%d %d}  ",j,NW[myId][j]);
		} else
			loop_count++;
		
		if (changed[myId])
			printf("\n__________________________________________\n");
	}
	
	return NULL;
}

void sendDV(int incId, int toId)
{
	//lock the receiver's queue
	mutex_lock(&lock[toId]);
	
	int i;
	//print out the data we are sending
	printf("Sending DV: from=%d, to=%d; ",incId,toId);
	for (i = 0; i < N; i++)
		printf("(%d %d)  ",i,NW[incId][i]);
	printf("\n");

	//Create new head/tail if none exist otherwise, add to tail
	if (Q_head[toId] == NULL) {
		Q_head[toId] = Q_tail[toId] = malloc (sizeof(struct dv));
		Q_head[toId]->next = Q_tail[toId]->next = NULL;
	} else {
		Q_tail[toId]->next = malloc (sizeof(struct dv));
		Q_tail[toId] = Q_tail[toId]->next;
	}
	Q_tail[toId]->sender = incId;
	Q_tail[toId]->next = NULL;
	
	//Setup the list of values that we are sending
	for (i = 0; i < N; i++)
		Q_tail[toId]->link[i] = NW[incId][i];
	
	//Unlock the queue
	mutex_unlock(&lock[toId]);
}

int processQ(int incId)
{
	//Lock the receivers queue until we are finished.
	mutex_lock(&lock[incId]);
	changed[incId] = 0;
	int i, tmpVal = 0,sender = 0,change = 0, tmpDV[N];
	
	//print sender then link list, then pop off the element
	while(Q_head[incId] != NULL)
	{
		changed[incId] = 1;
		sender = Q_head[incId]->sender;
		//Process node in the queue with output
		printf("Processing DV: by=%d, from=%d: ",incId,sender);
		for (i = 0; i < N; i++) {
			printf("(%d %d) ",i,Q_head[incId]->link[i]);
			tmpDV[i] = Q_head[incId]->link[i];
			//Calculate the new Value and determine if it is better that the previous
			tmpVal = tmpDV[i] + NW[incId][sender];
			if (tmpVal < NW[incId][i]) {
				NW[incId][i] = tmpVal;
				change = 1;
			}
		}
		printf("\n");
		
		//free up space from the printed/processed node
		node_ptr tmp = Q_head[incId];
		Q_head[incId] = Q_head[incId]->next;
		free(tmp);
	}
	
	mutex_unlock(&lock[incId]);
	return change?1:0;
}

int main(int argc, char* argv[])
{
	//If we don't have the appropriate # of arguments, exit.
	if (argc != 2) {
		printf("Usage is:\tSTAGE%d filename\n",2);
		return 1;
	}
	
	int i, j, status, m,from,to,dist;
	FILE *fp;
	
	//Initiate the head, tail and mutex needed by each thread
	for (i = 0; i < N; i++) {
		mutex_init(&lock[i],USYNC_THREAD,NULL);
		Q_head[i] = NULL;
		Q_tail[i] = NULL;
	}
	
	//Initialize dummy values for array
	for ( i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			NW[i][j] = 10000;
			
	//Open the file input by user and read in values to NW array
	fp=fopen(argv[1],"r");
	fscanf(fp,"%d",&m);
	for (i = 0; i < m; i++) {
		fscanf(fp,"%d %d %d",&from,&to,&dist);
		NW[from][to] = NW[to][from] = dist;
	}
		
	//Initialize all of the threads needed for the program
	for (i = 0; i < N; i++)
		thr_create(NULL,0,city,(void*)i, THR_BOUND, &thr[i]);
	
	//Wait for all the threads to finish
	while(thr_join(0,&tid, (void**)&status)==0);
	
	//Print the result
	printf("--------------------------------------------------------------\n");
	printf("SHORTEST DISTANCES CALCULATED USING DISTANCE VECTOR ALGORITHM:\n");
	printf("--------------------------------------------------------------\n  ");
	for (i = 0; i < N; i++)
		printf("%6d",i);
	printf("\n--------------------------------------------------------------\n");
	for (i = 0; i < N; i++){
		printf("%d:",i);
		for (j = 0; j < N; j++)
			printf("%6d",NW[i][j]);
		printf("\n");
	}
	printf("\n--------------------------------------------------------------\n");
	
	//Destroy each mutex
	for (i = 0; i < N; i++) mutex_destroy(&lock[i]);
	
	//Close the file
	fclose(fp);
	
	return 0;
}
