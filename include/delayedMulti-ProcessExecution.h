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
