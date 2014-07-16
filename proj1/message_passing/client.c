/**************************************************************/
/* Thomas Fox, CS 4352, Client program            Due: 4/8/93 */
/*                                                            */
/* The client sends a message to the server and then waits    */
/* for a reply.  The server MSGKEY is fixed in this stage.    */
/**************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "mymsg.h"

/* Two arbitrarily chosen keys. The server-key must be known to
client. Client can then obtain queue-id of the server. 
Client's queue-id is passed with request to server. 
The keys can be thought of as port number used in TCP/IP protocols. */

#define S_MSGKEY      8987
#define C_MSGKEY      0991

int myqid;


void main()
{
     void cleanup(); /* Failure recovery procedure */
     int servqid, each;
     int msgflag;
     int mtype;

     MSGTOSERVER to_server;
     MSGTOCLIENT to_client;

/* Below shows a very trivial processing in case, the client dies,
because it received a signal.  It shows failure handling.*/
     for (each = 0; each < 20; each++)   /* Kill queue when a signal occurs */
          signal (each, cleanup);

/* 0777 | IPC_CREAT denotes no read/write/execute restrictions;
create a queue if one does not exists associated to given KEY.  */
     msgflag = 0777|IPC_CREAT;
     myqid = msgget (C_MSGKEY, msgflag);/* Get own queue */
     printf ("client qid = %d\n", myqid); //for debugging

/* Get server's queue-id. Server is supposed to have open queue
before with no restrictions.*/
     msgflag = 0777;
     servqid = msgget (S_MSGKEY, msgflag); 
     printf ("server qid = %d\n", servqid);
     
/* Prepare request message.  The data is just the client's q-id. */
     to_server.mtype = 1;
     to_server.clienttext.code = myqid;  /* Any messgae to pass */

/* Client sends a request of message type 1 and awaits to receive
reply of any mtype. mtype = 0 denotes any type. 
perror is library function to print error. */
     mtype = 0; 
     msgflag = 0777;
     if (msgsnd (servqid, &to_server, sizeof (REQUEST), msgflag) == -1)
     {
          printf ("Client:  Request not sent.\n");
          perror (NULL);
     }
     else
     if (msgrcv (myqid, &to_client, sizeof (REPLY), mtype, msgflag) == -1)
     {
          printf ("Client:  Reply not received.\n");
          perror (NULL);
     }
     else
          printf ("Client:  Sent qid=%d, Received qid=%d\n",
                  myqid, to_client.servertext.reply);
}

/* This procedure is called automatically, when the process
receives any signals. */

void cleanup ()  /* Remove queue */
{
        msgctl (myqid, IPC_RMID, 0);
        exit(0);
}
