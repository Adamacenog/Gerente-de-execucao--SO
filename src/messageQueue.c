/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
#endif

int queueCreator(key_t key)
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

int messageReceive(int msqid, struct msgbuf *rbuf, long msgtype, int block)
{
  errno = 0;
  
  // Receives the message with type msgtype
  // msgrcv's flag is 0, so that it is blocked ultil a msg with type msgtype is in the queue
  if(block == 1) 
  {
    if (msgrcv(msqid, rbuf, MSGSZ, msgtype, 0) < 0 && errno != ENOMSG)
    {
      printf("Error receiving the message from queue.\n");
      perror("msgrcv");
      exit(1);
    }
  }
  else 
  {
    if (msgrcv(msqid, rbuf, MSGSZ, msgtype, IPC_NOWAIT) < 0 && errno != ENOMSG)
    {
      printf("Error receiving the message from queue.\n");
      perror("msgrcv");
      exit(1);
    }
  }

  // If no message was received, return 0, else, 1
  if (errno == ENOMSG)
    return 0;
  else
    return 1;   
}

void messageSend(int msqid, struct msgbuf sbuf, size_t buf_length)
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

void queueDestroy(int msqid)
{
  struct msqid_ds *buf;

  if ((msgctl(msqid, IPC_RMID, buf)) < 0)
  {
    printf("Error while destroying the message queue.\n");
    perror("msgctl");
    exit(1);
  }
}

void createMessage(int msqid, int jobId, char *seconds, char *execFile, long mtype)
{
  char jobIdString[10];
  struct msgbuf buf;
  int i;

  for (i = 0; i < MSGSZ; i++)
    buf.mtext[i] = 0;

  sprintf(jobIdString, "%d", jobId);

  strcpy(buf.mtext, jobIdString);
  strcat(buf.mtext, "|");
  strcat(buf.mtext, seconds);
  strcat(buf.mtext, "|");
  strcat(buf.mtext, execFile);

  buf.mtype = mtype;

  messageSend(msqid, buf, (strlen(buf.mtext) + 1));
}

int receivedDelayedJob(int msqid, int job_counter, struct Job *job_entry)
{
  struct msgbuf bufReceive;
  char exeFile[50], seconds[10], jobPid[10], pattern[2] = "|";

  /* Receives a msg from queue created by delayedMulti */
  if (messageReceive(msqid, &bufReceive, 666, 0))
  {
    /* Cuts the string with the pattern to be parsed */
    copyNremoveByPattern(jobPid, 10, bufReceive.mtext, 500, *pattern);
    copyNremoveByPattern(seconds, 10, bufReceive.mtext, 500, *pattern);
    copyNremoveByPattern(exeFile, 50, bufReceive.mtext, 500, *pattern);

    /* Initializes job values */
    (*job_entry).jobId = job_counter;
    (*job_entry).seconds = atoi(seconds);
    strcpy((*job_entry).exeFile, exeFile);
    (*job_entry).start_time = 0;
    (*job_entry).end_time = 0;
  
    return 1;
  }

  return 0;
}

void copyNremoveByPattern(char *destination, int sizeOfDest, char *source, int sizeOfSource, char pattern)
{
  int i;

  // Copies the dest to source until pattern.
  for (i = 0; i < sizeOfDest && i < sizeOfSource; i++)
  {
    if (source[i] == pattern || source[i] == '\0')
    {
      destination[i] = 0;
      break;
    }      
    
    destination[i] = source[i];
  }

  // Get's source length
  sizeOfDest = strlen(destination);

  // Erase from dest. the source
  for (i = 0; i < (sizeOfSource - sizeOfDest); i++)
  {
    source[i] = source[i + sizeOfDest + 1]; // Plus 1 because of pattern.
  }
}