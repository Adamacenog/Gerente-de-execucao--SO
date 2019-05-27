/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes
*/

#ifndef _Scheduler_library
  #define _Scheduler_library
    #include "scheduler.h"
#endif

// Those variables need to be global, so that the signal redefinition can use them.
int msqid;
struct JobQueue *jobQueueHead = NULL;
struct FinishedJobTable *finishedJobTableHead = NULL, *finishedJobTableTail = NULL;

int main(int argc, char *argv[])
{
  struct Job *job_entry;
  int i, pid[16], busyTable[16], job_counter, topologyType = -1, nodes_Size;
  char *topology, jobIdString[10];
  key_t key = 7869;

  signal(SIGALRM, delayed_message_send);
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
      nodes_Size = 16;
    }

    if (strcmp(topology, "torus") == 0)
    {
      topologyType = 2;
      nodes_Size = 16;
    }

    if (strcmp(topology, "fat_tree") == 0)
    {
      topologyType = 3;
      nodes_Size = 15;
    }

    free(topology);

    if (topologyType != -1)
    {
      /* Initializes message queue */
      msqid = queueCreator(key);

      /* Creates N process that will execute the process manager logic */
      for (i = 0; i < nodes_Size; i++)
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
            sprintf(jobIdString, "%d", (i + 1));
            execl("./gerente_execucao", "gerente_execucao", jobIdString, NULL);
            
          }
        }
      }
    }

    job_counter = 1;
    job_entry = (job *)malloc(sizeof(job));

    if (job_entry == NULL)
    {
      printf("Error on malloc.");
      exit(1);
    }

    while (1)
    {
      runScheduler(msqid, job_entry, &job_counter, jobIdString);
    }
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}

void runScheduler(int msqid, struct Job *job_entry, int *job_counter, char *jobIdString)
{
  int alarm_Remaining;

  if (receivedDelayedJob(msqid, (*job_counter), job_entry))
  {
    printf("scheduler-MENSSAGE: %s\n", (*job_entry).exeFile);
    printf("scheduler-SECONDS: %d\n", (*job_entry).seconds);
    alarm_Remaining = alarm(0);
    
    if (jobQueueHead != NULL)
    {
      decreaseAllRemainingTimes(jobQueueHead, ((*jobQueueHead).remainingSeconds) - alarm_Remaining);
    }    
    /* TO DO: FIX addToQueue - IS CAUSING SEG FAULT, try 2 sec, 2 sec, 3 sec*/
    addToQueue(&jobQueueHead, (*job_entry));
    printf("\nQueue:\n");
    printfJobToExecute(jobQueueHead);
    printf("\n");
    alarm((*jobQueueHead).remainingSeconds);
    (*job_counter)++;
  }

}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void delayed_message_send(int sig) 
{
  char seconds[10];

  if (jobQueueHead != NULL)
  {
    printf("ALARM INTERRUPT\n");
    decreaseAllRemainingTimes(jobQueueHead, (*jobQueueHead).remainingSeconds);
    
    while (jobQueueHead != NULL && (*jobQueueHead).remainingSeconds <= 0)
    {
      printf("EXECUTING JOB %d\n", (*jobQueueHead).job.jobId);
      sprintf(seconds, "%d", (*jobQueueHead).job.seconds);
      /* Message is created and sent to node 1 (whitch is node 0), with mtype 1 */
      createMessage(msqid, (*jobQueueHead).job.jobId, seconds, (*jobQueueHead).job.exeFile, 1);
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
  printf("\n\nShuting down...\n");
  
  if (jobQueueHead != NULL)
  {
    printf("Jobs that were waiting to start the execution:\n");
    printfJobToExecute(jobQueueHead);
    deleteQueue(&jobQueueHead);
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