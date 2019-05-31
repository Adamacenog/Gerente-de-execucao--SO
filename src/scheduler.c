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
int msqid, pid[16], busyNodes = 0, nodesSize;
struct JobQueue *jobQueueHead = NULL;
struct JobTable *finishedJobTableHead = NULL, *finishedJobTableTail = NULL, *job2ExecuteHead = NULL, *job2ExecuteTail = NULL;

int main(int argc, char *argv[])
{
  int i, jobCounter, topologyType = -1;
  char *topology, jobIdString[10], topologyString[30];
  key_t key = 7869;

  signal(SIGALRM, delayedMessageSend);
  signal(SIGTERM, terminateScheduler);

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

      /* Sends to the killScheduler the scheduler pid */
      sendPid(msqid, getpid());

      /* Creates N process that will execute the process manager logic */
      for (i = 0; i < nodesSize; i++)
      {
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
      runScheduler(msqid, &jobCounter);
    }
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}


void runScheduler(int msqid, int *jobCounter)
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
    printf("\nDelayed job queue:\n");
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

  if (busyNodes == 0)
  {
    if (job2ExecuteHead != NULL)
    {
      busyNodes = nodesSize;
      printf("\nEXECUTING JOB: %d\n\n", (*job2ExecuteHead).job.jobOrder);
      
      /* Message is created and sent to node 0 (using mtype 555) */
      createMessage(msqid, &((*job2ExecuteHead).job), 555);
      removeJobHead(&job2ExecuteHead, &job2ExecuteTail);
    }
  }

  /* Checks messages from node 0 */
  if (receiveMessage(msqid, &jobExit, 777))
  {
    addToJobTable(&finishedJobTableHead, &finishedJobTableTail, jobExit); 
    busyNodes -= 1; // Each new message from node 0 represents a node that is free

    if (busyNodes == 0)
    {
      printf("\nJob %d ended its execution!\n\nStatistics:\n\n", finishedJobTableHead->job.jobOrder);
      printfJobStatistics(finishedJobTableHead, finishedJobTableTail);
      deleteJobTable(&finishedJobTableHead,&finishedJobTableTail);
    }
  }
}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void delayedMessageSend(int sig) 
{
  if (jobQueueHead != NULL)
  {
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

  /* Checks if all nodes are idle */
  if (busyNodes == 0)
  {
    killAllNodes();
  }
  else
  {
    struct Job job;
    job.jobOrder = -1;
   
    /* If there is a node still executing, tell it to stop and send its statistics (endtime comes with -1) */
    /* Stop message is created and sent to node 0 (using mtype 555) - SOFT KILL MESSAGE */
    createMessage(msqid, &job, 555);
    
    while (busyNodes != 0)
    {
      if (receiveMessage(msqid, &job, 777))
      {
        addToJobTable(&finishedJobTableHead, &finishedJobTableTail, job); 
        busyNodes -= 1; // Each new message from node 0 represents a node that is free
      }
    }

    printf("\nJob %d was forced to end its execution!\n\nStatistics:\n\n", finishedJobTableHead->job.jobOrder);
    printfJobStatistics(finishedJobTableHead, finishedJobTableTail);
    deleteJobTable(&finishedJobTableHead,&finishedJobTableTail);

    killAllNodes();
  }  
  
  if (jobQueueHead != NULL)
  {
    printf("\nJobs that were still waiting to start the execution:\n");
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

  // Delete the message queue
  queueDestroy(msqid);

  exit(0);
}

void killAllNodes()
{
  int status_ptr, i, process2End = nodesSize;

  for (i = 0; i < nodesSize; i++)
  {
    if (kill(pid[i],SIGTERM) == -1)
    {
      printf("Error while sending signal.\n");
      perror("kill");
    }
  }

  while (process2End != 0)
  {
    if (waitpid(pid[process2End-1], &status_ptr, WUNTRACED) == pid[process2End-1])
    {
      if (WIFEXITED(status_ptr))
      {
        process2End -= 1;        
      }
      else
      {
        printf("Error, node did not finish correctly!\n");
      }
    }
    else
    {
      printf("Execution Error.\n");
      perror("waitpid");
    }  
  }
}

void sendPid(int msqid, pid_t pid)
{
  struct msgbuf buf;

  buf.mtype = 999;  // Type used to communicate with killScheduler
  sprintf(buf.mtext, "%d", pid);
  messageSend(msqid, buf, (strlen(buf.mtext) + 1));
}