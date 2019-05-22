/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Primary_libraries
  #define _Primary_libraries
    #include <stdio.h>
    #include <ctype.h>
    #include <string.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
#endif

#ifndef _ProcessManager_library
  #define _ProcessManager_library
    #include "processManager.h"
#endif

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
#endif

#ifndef _Job_Queue
  #define _Job_Queue
    #include "job.h"
#endif

void runScheduler(int, struct Job *, int, char *);
void delayed_message_send(int);