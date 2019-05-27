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
#endif


typedef struct Job
{
  int jobId;
  int seconds;
  time_t startTime;
  time_t endTime;
  char exeFile[50];
} job;

typedef struct FinishedJobTable
{
  struct Job job;
  struct FinishedJobTable *nextTable;
} finishedJobTable;

typedef struct JobQueue
{
  int remainingSeconds;
  struct Job job;
  struct JobQueue *next;
} jobQueue;

void addToQueue(jobQueue **, job);
void removeHead(jobQueue **);
void deleteQueue(jobQueue **);
void decreaseAllRemainingTimes(jobQueue *, int);
void addToJobTable(finishedJobTable **, finishedJobTable **, job);
void printfJobToExecute(jobQueue *);
void printfJobTable(finishedJobTable *);
void deleteJobTable(finishedJobTable **, finishedJobTable **);