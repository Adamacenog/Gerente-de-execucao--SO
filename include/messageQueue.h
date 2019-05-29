/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Primary_libraries
  #define _Primary_libraries
    #include <stdio.h>
    #include <ctype.h>
    #include <string.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
    #include <sys/wait.h>
#endif

#ifndef _Queue_Definition
  #define _Queue_Definition
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
#endif

#ifndef _Job_Queue
  #define _Job_Queue
    #include "job.h"
#endif

#define MSGSZ 500

// Declare the message structure.
typedef struct msgbuf
{
	long mtype;
	char mtext[MSGSZ];
} msgbuf;

int queueCreator(key_t);
int messageReceive(int, struct msgbuf *, long, int);
void messageSend(int, struct msgbuf, size_t);
void queueDestroy(int msqid);
void createMessage(int, struct Job *, long);
void copyNremoveByPattern(char *, int, char *, int, char);
int receiveMessage(int, struct Job *, long);
void convertBuf2Job(char *, struct Job *);
void convertJob2Buf(struct Job *, char *);