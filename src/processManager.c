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
#endif

#ifndef _ProcessManager_library
#define _ProcessManager_library
#include "processManager.h"
#endif

int main(int argc, char *argv[])
{
  char *topology;

  if (argc < 3 || argc > 3)
  {
    printf("Quantidade de argumentos inválida\n");
    exit(1);
  }

  if (argc == 2)
  {
    topology = (char *)malloc(sizeof(argv[1]));
    strcpy(topology, argv[1]);
    printf("%s\n", topology);
    topology = tolower(topology);

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
