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

int main(int argc, char *argv[])
{
  int i, msqid, pid[16], busyTable[16], job_counter, topologyType = -1, nodesSize;
  char *topology, jobIdString[10], pattern[2] = "|";
  char *seconds, execFile[10];
  key_t key = 7869;
  struct msgbuf bufReceive;
  struct Job* job_entry;

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
      msqid = QueueCreator(key);

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
            execl("./gerente_execucao", "gerente_execucao", jobIdString, NULL);
          }
        }
      }
    }

    signal(SIGALRM, delayed_message_send);
    job_counter = 1;

    while (1)
    {
      runScheduler(msqid, job_entry, bufReceive, job_counter, seconds, execFile, jobIdString, pattern);
    }
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}

void runScheduler(int msqid, struct Job*  job_entry, struct msgbuf bufReceive, int job_counter, char * seconds, char * execFile, char * jobIdString, char pattern)
{
  /* Receives a msg from queue created by delayedMulti */
  MessageReceive(msqid, &bufReceive, 666, 0);

  /* Cuts the string with the pattern to be parsed */
  strcpy(jobIdString,strtok(bufReceive.mtext,pattern));
  strcpy(seconds,strtok(bufReceive.mtext,pattern));
  strcpy(execFile,strtok(bufReceive.mtext,pattern));

  /* Initializes job values */
  job_entry->jobId = job_counter;
  job_entry->seconds = seconds;
  strcpy(job_entry->exeFile, execFile);
  job_entry->start_time = time(NULL);
  
  alarm(job_entry->seconds);
}

void delayed_message_send(int msqid, struct Job* job_entry) {
  /* TO DO: Send job_entry inside a message queue with another msqid */
  CreateMessage(msqid, job_entry->jobId, job_entry->seconds, job_entry->exeFile, 666);
}
