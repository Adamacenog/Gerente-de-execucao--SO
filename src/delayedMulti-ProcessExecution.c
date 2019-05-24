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

  if (argc == 3)
  {
    // Creates the queue that is used to communicate with scheduler
    msqid = queueCreator(key);

    // Sends the program exec file and delayed execution time to the scheduler (using the queue)
    createMessage(msqid, getpid(), argv[1], argv[2], mtype);
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}
