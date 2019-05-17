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

#ifndef _SHM_library
  #define _SHM_library
    #include "shm.h"
#endif

int main(int argc, char *argv[])
{
  int i;
  char *exeFile;
  struct job *jobHead = NULL;

  if (argc == 3)
  {
    exeFile = (char *)malloc(sizeof(argv[2]));

    if (exeFile == NULL)
    {
      printf("Error on malloc.");
      exit(1);
    }

    strcpy(exeFile, argv[2]);

    for (i = 0; i < sizeof(argv[2]) + 1; i++)
      *(exeFile + i) = tolower(*(exeFile + i));

    printf("%s\n", exeFile);


    free(exeFile);
  }
  else
  {
    printf("Invalid number of arguments.\n");
    exit(1);
  }

  return 0;
}
