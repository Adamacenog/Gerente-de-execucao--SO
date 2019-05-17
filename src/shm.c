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
#endif

#ifndef _SHM_Includes
  #define _SHM_Includes
    #include <sys/ipc.h>
    #include <sys/shm.h>
#endif

#ifndef _SHM_library
  #define _SHM_library
    #include "shm.h"
#endif

int SHMCreator(int size)
{
  int shmid;
  key_t key = 3139;

  if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0)
  {
    printf("Error while creating the shared memory.\n");
    perror("shmget");
    exit(1);
  }

  return shmid;
}

struct job * SHMAttach(int shmid)
{
  struct job *jobHead;

  // Attaches the shared memory with the process memory
  // shmat's *shmaddr & flag is 0, so that the operating system determines the memory shared address.
  if ((jobHead = (struct job *) shmat(shmid, 0, 0)) < 0)
  {
    printf("Error attaching the shared memory.\n");
    perror("shmat");
    exit(1);
  }

  return jobHead;
}


void SHMDestroy(int shmid)
{
  struct shmid_ds *buf;

  if (shmctl(shmid, IPC_RMID, buf) < 0)
  {
    printf("Error while destroying the shared memory.\n");
    perror("shmctl");
    exit(1);
  }
}
