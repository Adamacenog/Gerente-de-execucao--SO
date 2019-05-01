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
    #include <stdlib.h>
#endif

#ifndef _ProcessManager_library
  #define _ProcessManager_library
    #include "processManager.h"
#endif

int main(int argc, char *argv[])
{

  printf("Foram colocados %d argumentos\n", argc -1);

  return 0;
}
