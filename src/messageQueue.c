/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
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

#ifndef _Queue_Definition
  #define _Queue_Definition
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
#endif

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
#endif

int QueueCreator(key_t key)
{
  int msqid;

  if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0)
  {
    printf("Error while creating the message queue.\n");
    perror("msgget");
    exit(1);
  }

  return msqid;
}

void MessageReceive(int msqid, struct msgbuf *rbuf, long msgtype)
{
  // Receives the message with type msgtype
  // msgrcv's flag is 0, so that it is blocked ultil a msg with type msgtype is in the queue
  if (msgrcv(msqid, rbuf, MSGSZ, msgtype, 0) < 0)
  {
    printf("Error receiving the message from queue.\n");
    perror("msgrcv");
    exit(1);
  }
}

void MessageSend(int msqid, struct msgbuf sbuf, size_t buf_length)
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

void QueueDestroy(int msqid)
{
  struct msqid_ds *buf;

  if ((msgctl(msqid, IPC_RMID, buf)) < 0)
  {
    printf("Error while destroying the message queue.\n");
    perror("msgctl");
    exit(1);
  }
}
