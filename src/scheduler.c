/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Scheduler_library
  #define _Scheduler_library
    #include "scheduler.h"
#endif

// These variables need to be global, so that the signal redefinition can use them.
int msqid;
struct JobQueue *jobQueueHead = NULL;
struct JobTable *finishedJobTableHead = NULL, *finishedJobTableTail = NULL, *job2ExecuteHead = NULL, *job2ExecuteTail = NULL;

int main(int argc, char *argv[])
{
  int i, pid[16], busyTable[16], jobCounter, topologyType = -1, nodesSize, busyNodes = 0;
  char *topology, jobIdString[10], topologyString[10];
  key_t key = 7869;

  signal(SIGALRM, delayedMessageSend);
  signal(SIGTERM, terminateScheduler);

  printf("My pid: %d\n", getpid());

  if (argc == 2)
  {
    topology = (char *)malloc(sizeof(argv[1]));

    if (topology == NULL)
    {
      printf("Error on malloc.");
      exit(1);
    }

    strcpy(topology, argv[1]);

    for (i = 0; i < sizeof(argv[1]) + 1; i++)
      *(topology + i) = tolower(*(topology + i));

    printf("%s\n", topology);

    /* general process scheduler */

    if (strcmp(topology, "hypercube") == 0)
    {
      topologyType = 1;
      nodesSize = 16;
    }

    if (strcmp(topology, "torus") == 0)
    {
      topologyType = 2;
      nodesSize = 16;
    }

    if (strcmp(topology, "fat_tree") == 0)
    {
      topologyType = 3;
      nodesSize = 15;
    }

    free(topology);

    if (topologyType != -1)
    {
      /* Initializes message queue */
      msqid = queueCreator(key);

      /* Creates N process that will execute the process manager logic */
      for (i = 0; i < nodesSize; i++)
      {
        busyTable[i] = 0;

        // Checks if fork is created sucessfully
        if ((pid[i] = fork()) < 0)
        {
          printf("Error while creating a node.\n");
          perror("fork");
          exit(1);
        }
        else
        {
          // Checks whether it is processes father (pid != 0) or child (pid == 0)
          if (pid[i] == 0)
          {
            sprintf(jobIdString, "%d", i);
            sprintf(topologyString, "%d", topologyType);
            execl("./gerente_execucao", "gerente_execucao", jobIdString, topologyString, NULL);            
          }
        }
      }
    }

    jobCounter = 1;

    while (1)
    {
      runScheduler(msqid, &jobCounter, &busyNodes);
    }
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}


void runScheduler(int msqid, int *jobCounter, int *busyNodes)
{
  int alarmRemaining;
  struct Job jobEntry, jobExit;

  /* Checks if there are any new jobs to be delayed/executed - sent by delayedMulti-ProcessExecution */
  if (receiveMessage(msqid, &jobEntry, 666))
  {
    jobEntry.jobOrder = *jobCounter;
    alarmRemaining = alarm(0);
    
    if (jobQueueHead != NULL)
    {
      decreaseAllRemainingTimes(jobQueueHead, ((*jobQueueHead).remainingSeconds) - alarmRemaining);
    }    
    
    addToQueue(&jobQueueHead, jobEntry);
    printf("\nQueue:\n");
    printfJobToExecute(jobQueueHead);
    printf("\n");

    /* handle the jobs that have remaining seconds == 0  or less, adding them to the execution queue */
    while (jobQueueHead != NULL && (*jobQueueHead).remainingSeconds <= 0)
    {
      /* Adds the job that has been delayed to the execution queue */
      addToJobTable(&job2ExecuteHead, &job2ExecuteTail, (*jobQueueHead).job);
      removeHead(&jobQueueHead);
    }

    if (jobQueueHead != NULL)
    {
      alarm((*jobQueueHead).remainingSeconds);
    }

    (*jobCounter)++;
  }

  /* Checks messages from node 0 */
  if (receiveMessage(msqid, &jobExit, 777))
  {
    addToJobTable(&finishedJobTableHead, &finishedJobTableTail, jobExit); 
    *busyNodes -= 1; // Each new message from node 0 represents a node that is free
  }

  if (*busyNodes == 0)
  {
    if (job2ExecuteHead != NULL)
    {
      *busyNodes = 16;
      //printf("EXECUTING JOB %d\n", (*job2ExecuteHead).job.jobOrder);
      
      /* Message is created and sent to node 0 (using mtype 555) */
      createMessage(msqid, &((*job2ExecuteHead).job), 555);
      removeJobHead(&job2ExecuteHead);
    }
  }
}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void delayedMessageSend(int sig) 
{
  if (jobQueueHead != NULL)
  {
    printf("ALARM INTERRUPT\n");
    decreaseAllRemainingTimes(jobQueueHead, (*jobQueueHead).remainingSeconds);
    
    while (jobQueueHead != NULL && (*jobQueueHead).remainingSeconds <= 0)
    {
      /* Adds the job that has been delayed to the execution queue */
      addToJobTable(&job2ExecuteHead, &job2ExecuteTail, (*jobQueueHead).job);
      removeHead(&jobQueueHead);
    }

    if (jobQueueHead != NULL)
    {
      alarm((*jobQueueHead).remainingSeconds);
    }      
  }
}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void terminateScheduler(int sig)
{
  printf("\n\nShutting down...\n");

  /* TO DO: 'KILL' all nodes, call 'wait' for all nodes... */
  
  if (jobQueueHead != NULL)
  {
    printf("Jobs that were waiting to start the execution:\n");
    printfJobToExecute(jobQueueHead);
    deleteQueue(&jobQueueHead);
  }

  if (job2ExecuteHead != NULL)
  {
    printf("\n\n");
    printf("Jobs that were ready to execute, waiting for the nodes to be free:\n");
    printfJobTable(job2ExecuteHead);
    deleteJobTable(&job2ExecuteHead, &job2ExecuteTail);
  }

  printf("\n\n");
  printf("Nodes statistics execution table:\n");

  if (finishedJobTableHead != NULL)
  {
    printfJobTable(finishedJobTableHead);
    deleteJobTable(&finishedJobTableHead, &finishedJobTableTail);
  }
  else
  {
    printf("No jobs were executed!\n");
  }

  // Delete the message queue
  queueDestroy(msqid);

  exit(0);
}