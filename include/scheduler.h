/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes
*/

#ifndef _Scheduler_library
  #define _Scheduler_library
    #include "scheduler.h"
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