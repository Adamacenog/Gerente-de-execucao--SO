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

// isNode is used to differentiate from a node.
/*
createMessage mounts the message to be send to a specific
message queue
*/
void createMessage(int msqid, struct Job *jobEntry, long mtype)
{
  struct msgbuf buf;

  memset(buf.mtext,0,MSGSZ);
  convertJob2Buf(jobEntry, buf.mtext);

  buf.mtype = mtype;
  printf("Message sent: %s\n", buf.mtext);
  messageSend(msqid, buf, (strlen(buf.mtext) + 1));
}

void copyNremoveByPattern(char *destination, int sizeOfDest, char *source, int sizeOfSource, char pattern)
{
  int i;

  // Copies the source to dest until pattern.
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

  // Erase from source the dest.
  for (i = 0; i < (sizeOfSource - sizeOfDest); i++)
  {
    source[i] = source[i + sizeOfDest + 1]; // Plus 1 because of pattern.
  }
}

int receiveMessage(int msqid, struct Job *jobExit, long mtype)
{
  struct msgbuf bufReceive;

  /* Receives a msg with mtype # */
  if (messageReceive(msqid, &bufReceive, mtype, 0))
  {
    printf("BEFORE\n");
    convertBuf2Job(bufReceive.mtext, jobExit);  
    return 1;
  }

  return 0;
}

void convertBuf2Job(char *bufReceive, struct Job *jobExit)
{
  char auxString[50], pattern[2] = "|";
  time_t startTime, endTime;
  printf("FIRST\n");
  /* Cuts the string with the pattern to be parsed and Sets the job values */
    memset(auxString,0,50);
    printf("SECOND\n");
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    printf("STRING: '%s'\n", auxString);
    printf("JobExit-NodeId: %d\n", (*jobExit).nodeId);
    (*jobExit).nodeId = atoi(auxString);
    printf("SECOND\n");
    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    (*jobExit).nodePid = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    (*jobExit).delayedPid = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    (*jobExit).jobPid = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    (*jobExit).jobOrder = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    (*jobExit).seconds = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    
    if (sscanf(auxString, "%ld", &(*jobExit).startTime) == EOF)
    {
      printf("Error while converting the message starttime.\n");
      perror("msgctl");
      exit(1);
    }

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    if (sscanf(auxString, "%ld", &(*jobExit).endTime) == EOF)
    {
      printf("Error while converting the message endtime.\n");
      perror("msgctl");
      exit(1);
    }

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive, 500, *pattern);
    strcpy((*jobExit).exeFile, auxString);
    printf("THIRD\n");
}

void convertJob2Buf(struct Job *jobEntry, char *buf)
{
  char auxString[10];

  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).nodeId);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).nodePid);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).delayedPid);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).jobPid);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).jobOrder);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%d", (*jobEntry).seconds);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%ld", (*jobEntry).startTime);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  sprintf(auxString, "%ld", (*jobEntry).endTime);
  strcat(buf, auxString);
  strcat(buf, "|");
  memset(auxString,0,10);

  strcat(buf, (*jobEntry).exeFile);
}