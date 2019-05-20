/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
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
#include <unistd.h>
#endif

#ifndef _ProcessManager_library
#define _ProcessManager_library
#include "processManager.h"
#endif

#ifndef _Scheduler_library
#define _Scheduler_library
#include "scheduler.h"
#endif

#ifndef _Queue_library
#define _Queue_library
#include "messageQueue.h"
#endif

int main(int argc, char *argv[])
{
  int i, msqid, pid[16], busyTable[16], job_counter;
  char *topology, jobIdString[10], pattern[2] = "|";
  char *seconds, execFile[10];
  key_t key, processExecutionKey = 7869;
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
      key = 4915;

      /* Initializes message queue */
      msqid = QueueCreator(key);

      /* Creates N process that will execute the process manager logic */
      for (i = 0; i < 2; i++)
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
          else {
            /* Receives a msg from queue created by delayedMulti */
            msqid = QueueCreator(processExecutionKey);
            MessageReceive(msqid, &bufReceive, 666, 0);

            /* Cuts the string with the pattern to be parsed */
            strcpy(jobIdString,strtok(bufReceive.mtext,pattern));
            strcpy(seconds,strtok(bufReceive.mtext,pattern));
            strcpy(execFile,strtok(bufReceive.mtext,pattern));

            /* Initializes job values */
            job_entry->jobId = job_counter;
            job_entry->seconds = seconds;
            strcpy(job_entry->exeFile, execFile);

            /* TO DO: Send job_entry inside a message queue with another msqid */
            CreateMessage(msqid, job_entry->jobId, job_entry->seconds, job_entry->exeFile, 666);

            QueueDestroy(msqid);
          }
        }
      }
    }

    if (strcmp(topology, "torus") == 0)
    {
      key = 4916;
    }

    if (strcmp(topology, "fat_tree") == 0)
    {
      key = 4917;
    }

    free(topology);
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}
