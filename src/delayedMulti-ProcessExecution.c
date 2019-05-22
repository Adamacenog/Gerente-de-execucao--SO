/*
Propriedade de:
Alex Nascimento
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

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
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
    msqid = QueueCreator(key);

    // Sends the program exec file and delayed execution time to the scheduler (using the queue)
    CreateMessage(msqid, getpid(), argv[1], argv[2], mtype);
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}
