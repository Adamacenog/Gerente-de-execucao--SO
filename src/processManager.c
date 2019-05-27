/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _ProcessManager_library
  #define _ProcessManager_library
    #include "processManager.h"
#endif

int main(int argc, char const *argv[]) { 
 
    key_t key = 7869; 
    struct msgbuf pmBuffer; 
    int processManagerId, msqid; 
    char execFile[10], *seconds, pattern[2] = "|"; 
     
    if(argc == 2) { 
        signal(SIGTERM, endExecution); 
        processManagerId = atoi(argv[1]); 
        printf("PROCESS MANAGER ID: %d\n", processManagerId);
        /* If processManagerId == 1, it accesses scheduler queue */ 
        if(processManagerId == 1) { 
           msqid = queueCreator(key); 
 
            /* Receives a msg from queue created by delayedMulti */ 
           if (messageReceive(msqid, &pmBuffer, 1, 0)) 
           { 
               printf("BUFFER: %s", pmBuffer.mtext);
                /* Cuts the string with the pattern to be parsed */ 
                strcpy(seconds,strtok(pmBuffer.mtext,pattern)); 
                strcpy(execFile,strtok(pmBuffer.mtext,pattern)); 
           } 
 
        } 
        else { 
 
        } 
 
         
 
    }  
     
    else { 
        printf("Unsificcient argument numbers."); 
        exit(1); 
    } 
 
     
     
    return 0; 
}

// The function needs to receive an 'int', to describe what type of signal it is redefining
void endExecution(int sig)
{

}