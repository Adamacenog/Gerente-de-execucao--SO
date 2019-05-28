/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _delayedMulti_ProcessExecution_library
  #define _delayedMulti_ProcessExecution_library
    #include "delayedMulti-ProcessExecution.h"
#endif

int main(int argc, char *argv[])
{
  int i, msqid;
  // TYPE 666 IS USED TO COMUNICATE WITH SCHEDULER.
  long mtype = 666;
  key_t key = 7869;
  struct Job job;

  if (argc == 3)
  {
    // Creates the queue that is used to communicate with scheduler
    msqid = queueCreator(key);

    // Sets the job parameters
    job.nodeId = 0;
    job.nodePid = 0;
    job.delayedPid = getpid();
    job.jobPid = 0;       // pid of the job that was executed by the processManager
    job.jobOrder = 0;     // order that delayedMulti-ProcessExecution sent the job
    job.seconds = atoi(argv[1]);
    job.startTime = 0;
    job.endTime = 0;
    strcpy(job.exeFile, argv[2]);

    // Sends the program exec file and delayed execution time to the scheduler (using the queue)
    createMessage(msqid, &job, mtype);
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}
