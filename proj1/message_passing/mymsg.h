/* mymsg.h header file; must be included in both client.c and
server.c */

/* Definition of request structure. It is divided in two parts:
 First part is application dependent. In this example, it is just
 an integer. Second part denotes request structure.*/

typedef struct tagREQUEST
{
  char name[16];// could be very complex
} REQUEST;

/*mtype describes a code; client-server can trade messages of
different types, given by this code. In this application, it is
set to 1. */


typedef struct tagMSGTOSERVER
{
     long mtype; 
     REQUEST clienttext;
} MSGTOSERVER;
/*-----------------------------------------------------*/
/* Definition of reply structures. */
typedef struct tagREPLY
{
     int reply; //could be any thing
} REPLY;

typedef struct tagMSGTOCLIENT
{
     long mtype;
     REPLY servertext;
} MSGTOCLIENT;
