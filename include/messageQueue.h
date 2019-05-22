/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/
#ifndef _Queue_Definition
  #define _Queue_Definition
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
#endif

#define MSGSZ 500

// Declare the message structure.
typedef struct msgbuf
{
	long mtype;
	char mtext[MSGSZ];
} msgbuf;

int QueueCreator(key_t);
void MessageReceive(int, struct msgbuf *, long, int);
void MessageSend(int, struct msgbuf, size_t);
void QueueDestroy(int msqid);
void CreateMessage(int, int, char *, char *, long);
