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

    int process_manager_id, msqid;
    int neighboords[4];
    key_t key = 7869;
    
    if(argc == 2) {
        signal(SIGTERM, endExecution);
        process_manager_id = atoi(argv[1]);

        /* If process_manager_id == 0, it accesses scheduler queue */
        if(process_manager_id == 0) {
           msqid = QueueCreator(key);
           if(msqid < 0) {
               printf("Wrong initializion of Message Queue\n");
               exit(0);
           }
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