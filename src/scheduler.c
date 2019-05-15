/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves
Guilherme Lopes
*/

#ifndef _Primary_libraries
#define _Primary_libraries
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#endif

#ifndef _ProcessManager_library
#define _ProcessManager_library
#include "processManager.h"
#endif

#ifndef _Scheduler_library
#define _Scheduler_library
#include "scheduler.h"
#endif

#ifndef _Queue_library
#define _Queue_library
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

#define MSGSZ 128

int QueueCreator()
{
  int msqid;
  key_t key = 3139;

  if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0)
  {
    printf("Error while creating the message queue.\n");
    perror("msgget");
    exit(1);
  }
}

char *MessageReceive(int msqid, msgbuf rbuf, long msgtype)
{
  // Receives the message with type msgtype
  // msgrcv's flag is 0, so that it is blocked ultil a msg with type msgtype is in the queue
  if (msgrcv(msqid, &rbuf, MSGSZ, msgtype, 0) < 0)
  {
    printf("Error receiving the message from queue.\n");
    perror("msgrcv");
    exit(1);
  }

  return rbuf.mtext;
}

void MessageSend(int msqid, msgbuf sbuf, size_t buf_length)
{
  // Sends the message in sbuf (with it's type and content)
  // msgsnd's flag is IPC_NOWAIT, so it is not a blocking call
  if ((msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT)) < 0)
  {
    // printf("%d, %ld, %s, %ld\n", msqid, sbuf.mtype, sbuf.mtext, buf_length); // Debbuging
    printf("Error while sending the message to the queue.\n");
    perror("msgsnd");
    exit(1);
  }
}

void QueueDestroy(int msqid, struct msqid_ds msqid_ds)
{
  if ((msgctl(msqid, IPC_RMID, &msqid_ds)) < 0)
  {
    printf("Error while destroying the message queue.\n");
    perror("msgsnd");
    exit(1);
  }
}
