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
struct JobQueue *job_Queue_Head;

int main(int argc, char *argv[])
{
  struct Job *job_entry;
  int i, pid[16], busyTable[16], job_counter, topologyType = -1, nodes_Size;
  char *topology, jobIdString[10];
  key_t key = 7869;

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

    signal(SIGALRM, delayed_message_send);
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
    /*alarm_Remaining = alarm(0);
    
    if (job_Queue_Head != NULL)
    {
      decreaseAllRemainingTimes(job_Queue_Head, ((*job_Queue_Head).remainingSeconds) - alarm_Remaining);
    }    */
    /* TO DO: FIX addToQueue - IS CAUSING SEG FAULT, try 2 sec, 2 sec, 3 sec*/
    addToQueue(&job_Queue_Head, (*job_entry));
    printf("\nQueue:\n");
    printfJobToExecute(job_Queue_Head);
    printf("\n");
    //alarm((*job_Queue_Head).remainingSeconds);
    (*job_counter)++;
  }

}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void delayed_message_send(int sig) 
{
  char seconds[10];

  if (job_Queue_Head != NULL)
  {
    printf("EXECUTING JOB %d\n", (*job_Queue_Head).job.jobId);
    decreaseAllRemainingTimes(job_Queue_Head, (*job_Queue_Head).remainingSeconds);

    while (job_Queue_Head != NULL && (*job_Queue_Head).job.seconds <= 0)
    {
      sprintf(seconds, "%d", (*job_Queue_Head).job.seconds);
      /* TO DO: Send job_Queue_Head inside a message queue with another mtype */
      createMessage(msqid, (*job_Queue_Head).job.jobId, seconds, (*job_Queue_Head).job.exeFile, 1);
      removeHead(&job_Queue_Head);
    }

    if (job_Queue_Head != NULL)
    {
      alarm((*job_Queue_Head).remainingSeconds);
    }      
  }
}