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
     
    if(argc == 2) 
    { 
        /* Ends if it receives a SIGTERM */
        signal(SIGTERM, endExecution); 
        /* Receive the node id through args  */
        processManagerId = atoi(argv[1]); 
        printf("PROCESS MANAGER ID: %d\n", processManagerId);
    
        msqid = queueCreator(key); 

        /* Receives a msg from queue created by delayedMulti */ 
        if (messageReceive(msqid, &pmBuffer, 1, 0)) 
        { 
            printf("BUFFER: %s", pmBuffer.mtext);
            /* Cuts the string with the pattern to be parsed */ 
            strcpy(seconds,strtok(pmBuffer.mtext,pattern)); 
            strcpy(execFile,strtok(pmBuffer.mtext,pattern)); 
        } 

        /* Idea: convert processManagerId to binary, then check which nodes
        are its neibourghs. Flood the message to them, adding to your node list the
        message unique ID, and after that, start within a infinite loop to hear from the 
        queue if there are any messages there.


        list:
        int nodes[16] = 0;  // 0 means the node has never sent back this ID, 1 means
                            // that the node already has sent the statistics table back
                            // (trying to communicate to node 0).
        int UniqueID = ####;

        // message: destination, source, message (jobId...)

        */ 

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

void convertToBinary(char *dest, int source)
{
  int i, k;
  
  memset(dest,0,strlen(dest));
 
  for (i = 15; i >= 0; i--)
  {
    k = source >> i;
 
    if (k & 1)
      strcat(dest, "1");
    else
      strcat(dest, "0");
  }

}