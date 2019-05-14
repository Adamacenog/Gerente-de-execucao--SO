/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves
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

#ifndef _ProcessManager_library
  #define _ProcessManager_library
    #include "processManager.h"
#endif

#ifndef _Scheduler_library
  #define _Scheduler_library
    #include "scheduler.h"
#endif

int main(int argc, char *argv[])
{
  int i;
  char *topology;

  if (argc == 3)
  {
    topology = (char *)malloc(sizeof(argv[1]));
    strcpy(topology, argv[1]);

    for(i = 0; i < sizeof(argv[1]) + 1; i++)
      *(topology + i) = tolower(*(topology + i));

    printf("%s\n", topology);

    /* general process scheduler */
    // falha com a quantidade de argumentos errada

    if (strcmp(topology, "hypercube") == 0)
    {
    }

    if (strcmp(topology, "torus") == 0)
    {
    }

    if (strcmp(topology, "fat_tree") == 0)
    {
    }

    free(topology);
  }
  else
  {
    printf("Quantidade de argumentos inválida\n");
    exit(1);
  }

  return 0;
}
