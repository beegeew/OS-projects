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

void sendDV(int, int);
void processQ(int);

void *city(void *arg)
{
	int myId = (int) arg;
	
	int k, j, m;
	unsigned int DV[N];
	
	Q_head[myId]->sender = myId;
	
	printf("Thread ID %d distances are: ",myId);
	
	for (j = 0; j < N; j++)
	{
		mutex_lock(&lock[myId]);
		DV[j] = NW[myId][j];
		if (j == myId)
			DV[j] = 0;
		mutex_unlock(&lock[myId]);
		
		printf("%7d",DV[j]);
	}
	printf("\n");
	
	for (k = 0; k < N; k++)
		Q_head[myId]->link[k] = DV[k];
	
	for (j = 0; j < N; j++)
		if (j != myId) sendDV(myId, j);
	
	nanosleep(&Tim, NULL);
	
	processQ(myId);
	
	return NULL;
}

void sendDV(int incId, int toId)
{
	if (Q_tail[toId] == NULL) {
		Q_head[toId]->next = malloc (sizeof(struct dv));
		Q_tail[toId] = Q_head[toId]->next;
	} else {
		Q_tail[toId]->next = malloc (sizeof(struct dv));
		Q_tail[toId] = Q_tail[toId]->next;
	}
	Q_tail[toId]->sender = incId;
	Q_tail[toId]->next = NULL;
	
	int i;
	for (i = 0; i < N; i++)
		Q_tail[toId]->link[i] = Q_head[incId]->link[i];
}

void processQ(int incId)
{
	int i;
	
	printf("City #%d\n",incId);
	
	//print sender then link list, then pop off the element
	while(Q_head[incId] != NULL)
	{
		printf("Sender # %d  DV:",Q_head[incId]->sender);
		for (i = 0; i < N; i++)
			printf("%7d",Q_head[incId]->link[i]);
		
		printf("\n");
		
		Q_head[incId] = Q_head[incId]->next;
	}
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
		Q_head[i] = malloc(sizeof(struct dv));
		Q_head[i]->sender = i;
		Q_head[i]->next = NULL;
		Q_tail[i] = NULL;
	}
	
	//Initialize dummy values for array
	for ( i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			NW[i][j] = 10000;
			
	//Open the file input by user and read in values to NW array
	fp=fopen(argv[1],"r");
	fscanf(fp,"%d",&m);
	for (i = 0; i < N; i++) {
		fscanf(fp,"%d %d %d",&from,&to,&dist);
		NW[from][to] = dist;
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
