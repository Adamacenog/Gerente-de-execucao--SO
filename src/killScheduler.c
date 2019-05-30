/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _killScheduler_library
  #define _killScheduler_library
    #include "killScheduler.h"
#endif

int main(int argc, char *argv[])
{
    int i, msqid;
    // TYPE 999 IS USED TO COMUNICATE WITH SCHEDULER.
    long mtype = 999;
    key_t key = 7869;
    pid_t pid;
    struct msgbuf rbuf;

    // Creates the queue that is used to communicate with scheduler
    msqid = queueCreator(key);

    /* Gets the scheduler pid */
    messageReceive(msqid, &rbuf, mtype, 1);

    pid = atoi(rbuf.mtext);

    /* Kill Scheduler */
    if (kill(pid,SIGTERM) == -1)
    {
      printf("Error while sending signal.\n");
      perror("kill");
    }

  return 0;
}
