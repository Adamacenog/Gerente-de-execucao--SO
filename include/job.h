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
  int nodeId;       // Unique nodeId
  int nodePid;      // pid of the processManager node
  int delayedPid;   // pid of the delayedMulti-ProcessExecution that sent the job to be executed
  int jobPid;       // pid of the job that was executed by the processManager
  int jobOrder;     // order that delayedMulti-ProcessExecution sent the job
  int seconds;
  time_t startTime;
  time_t endTime;
  char exeFile[50];
} job;

typedef struct JobTable
{
  struct Job job;
  struct JobTable *nextTable;
} jobTable;

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
void addToJobTable(jobTable **, jobTable **, job);
void printfJobToExecute(jobQueue *);
void printfJobTable(jobTable *);
void deleteJobTable(jobTable **, jobTable **);
void removeJobHead(jobTable **);