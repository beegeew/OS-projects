/**************************************************************/
/* Thomas Fox, CS 4352, Server program            Due: 4/8/93 */
/*                                                            */
/* The server waits for a request from the client.  When it   */
/* receives the request, it replies to the client.            */
/**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "mymsg.h"
#define S_MSGKEY      7654

int myqid;
extern int errno;

void main()
{
     void cleanup();
     int cliqid, each;
     int msgflag, mtype;

     MSGTOSERVER to_server;
     MSGTOCLIENT to_client;

/* Trivial failure handling */
     for (each = 0; each < 20; each++)  /* Kill queue when a signal occurs */
          signal (each, cleanup);


/* Remove the old queue if any; create a new queue. 0777 allows
any one to send request who knows the server-key. */
     msgflag = 0777 | IPC_CREAT;
     myqid = msgget (S_MSGKEY, msgflag);
     if (myqid == -1) perror (NULL);
     printf ("server qid = %d\n", myqid);


     /* Generally, the server contains an infinite loop to
     receive request of any type, process it and send reply.
     Processing is not shown here. 
     Following msgrcv will block the server until a
     request is received.*/

     mtype = 0; //request of any type
     msgflag = 0 ;
     if (msgrcv (myqid, &to_server, sizeof (REQUEST), mtype, msgflag) == -1)
     {
          printf ("Server:  Request not received.\n");
          perror (NULL);
     }
     else {
     /* Request arrived, Prepare reply. In this example, 
     it is the server's qid; it could be just anything.*/
         sleep (2); /* To simulate processing */
         to_client.mtype = 1;
         to_client.servertext.reply = myqid;
	 if (msgsnd (to_server.clienttext.code, &to_client,
              sizeof (REPLY), 0) == -1)
          {
          printf ("Server:  Reply not sent.\n");
          perror (NULL);
          }
         else
          printf ("Server:  My qid=%d, Received qid=%d\n",
                  myqid, to_server.clienttext.code);
          }
 }

/* Called when a signal is received. */
void cleanup ()  /* Remove the queue */
{
        msgctl (myqid, IPC_RMID, 0);
        exit(0);
}
