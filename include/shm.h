/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes
*/

typedef struct Job
{
  int jobId;
  int seconds;
  char *exeFile;
  struct job *nextJob;
} job;

int SHMCreator(int);
struct job * SHMAttach(int);
void SHMDestroy(int shmid);
