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

#ifndef _ProcessManager_library
  #define _ProcessManager_library
    #include "processManager.h"
#endif

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
#endif

typedef struct Job
{
  int jobId;
  /* TO DO: change back to integer */
  char* seconds;
  time_t start_time;
  time_t end_time;
  char *exeFile;
} job;
