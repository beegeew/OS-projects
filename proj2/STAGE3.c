/*
CS4352
Project 2 - Beverly G Wertz
STAGE3
Calculating an optimal path cost via threads representing links between cities
*/

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
struct timespec Tim={0,10000000};

//Struct to hold each city's data
typedef struct dv
{	int sender;
	unsigned int link[N];
	struct dv* next;
}	node, *node_ptr;

node_ptr Q_head[N], Q_tail[N];

void sendDV(int, int);
int processQ(int);

void *city(void *arg)
{
	int myId = (int) arg;
	
	int k, j, m;
	unsigned int DV[N],CN[N];
	
	for (j = 0; j < N; j++)
	{
		mutex_lock(&lock[myId]);
		DV[j] = NW[myId][j];
		if (DV[j] < 10000) CN[j] = 1;
		else CN[j] = 0;
		
		if (j == myId)
			NW[myId][j] = DV[j] = 0;
		mutex_unlock(&lock[myId]);
	}
	
	printf("Neighbors of city=%d are: ",myId);
	for (j = 0; j < N; j++) if (CN[j] == 1) printf("%d, ",j);
	printf("\nInitial DV of city=%d;  ",myId);
		
	for (k = 0; k < N; k++) {
		printf("(%d %d)  ",k,DV[k]);
	}
	printf("\n\n");
	
	for (j = 0; j < N; j++)
		if (j != myId && CN[j] == 1) sendDV(myId, j);
	printf("*********************************************\n");
	
	nanosleep(&Tim, NULL);
	
	if (processQ(myId)) {
		printf("DV of %d changed:  ",myId);
		for (j = 0; j < N; j++)
			printf("(%d %d}  ",j,NW[myId][j]);
	}
	printf("\n__________________________________________\n");
	
	return NULL;
}

void sendDV(int incId, int toId)
{
	mutex_lock(&lock[toId]);
	
	int i;
	printf("Sending DV: from=%d, to=%d; ",incId,toId);
	for (i = 0; i < N; i++)
		printf("(%d %d)  ",i,NW[incId][i]);
	printf("\n");

	if (Q_head[toId] == NULL) {
		Q_head[toId] = Q_tail[toId] = malloc (sizeof(struct dv));
		Q_head[toId]->next = Q_tail[toId]->next = NULL;
	} else {
		Q_tail[toId]->next = malloc (sizeof(struct dv));
		Q_tail[toId] = Q_tail[toId]->next;
	}
	Q_tail[toId]->sender = incId;
	Q_tail[toId]->next = NULL;
	
	for (i = 0; i < N; i++)
		Q_tail[toId]->link[i] = NW[incId][i];
	
	mutex_unlock(&lock[toId]);
}

int processQ(int incId)
{
	mutex_lock(&lock[incId]);
	int i, tmpVal = 0,sender = 0,change = 0, tmpDV[N];
	
	//print sender then link list, then pop off the element
	while(Q_head[incId] != NULL)
	{
		sender = Q_head[incId]->sender;
		
		printf("Processing DV: by=%d, from=%d: ",incId,sender);
		for (i = 0; i < N; i++) {
			printf("(%d %d) ",i,Q_head[incId]->link[i]);
			tmpDV[i] = Q_head[incId]->link[i];
			
			tmpVal = tmpDV[i] + NW[incId][sender];
			if (tmpVal < NW[incId][i]) {
				NW[incId][i] = tmpVal;
				change = 1;
			}
		}
		printf("\n");
		
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
	
	//Destroy the mutex
	for (i = 0; i < N; i++) mutex_destroy(&lock[i]);
	
	//Close the file
	fclose(fp);
	
	return 0;
}
