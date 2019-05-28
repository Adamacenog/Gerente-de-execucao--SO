/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Job_Queue
  #define _Job_Queue
    #include "job.h"
#endif

void addToQueue(jobQueue **jobHead, job job)
{
    jobQueue *aux, *aux2, *aux3;
    
    aux = (jobQueue*)malloc(sizeof(jobQueue));    

    if (aux == NULL)
    {
        printf("Error on malloc.");
        exit(1);
    }
    
    aux->remainingSeconds = job.seconds;
    aux->job.jobOrder = job.jobOrder;
    aux->job.seconds = job.seconds;
    aux->job.startTime = job.startTime;
    aux->job.endTime = job.endTime;
    strcpy(aux->job.exeFile, job.exeFile);
    aux->next = NULL;

    if ((*jobHead) == NULL)
    {
        (*jobHead) = aux;
    }
    else
    {
        aux2 = (*jobHead);

        while(aux2->next != NULL && aux2->remainingSeconds < job.seconds)
        {
            aux3 = aux2;
            aux2 = aux2->next;
        }

        if (aux2 == (*jobHead))
        {
            if (aux2->remainingSeconds > job.seconds)
            {
                aux->next = (*jobHead);
                (*jobHead) = aux;
            }
            else
            {
                (*jobHead)->next = aux;
            }            
        }
        else
        {
            if (aux2->remainingSeconds > job.seconds)
            {
                aux->next = aux2;
                aux3->next = aux;
            }
            else
            {
                aux2->next = aux;
            } 
        }
    }
}

void removeHead(jobQueue **jobHead)
{
    jobQueue *aux;

    if ((*jobHead) != NULL)
    {
        aux = (*jobHead);
        (*jobHead) = (*jobHead)->next;
        free(aux);
    }
}

void deleteQueue(jobQueue **jobHead)
{
    while((*jobHead) != NULL)
    {
        removeHead(jobHead);
    }
}

void decreaseAllRemainingTimes(jobQueue *jobHead, int amount2Decrease)
{
    while(jobHead != NULL)
    {
        jobHead->remainingSeconds -= amount2Decrease;
        jobHead = jobHead->next;
    }
}

void addToJobTable(finishedJobTable **finishedJobTableHead, finishedJobTable **finishedJobTableTail, job finishedJob)
{
    finishedJobTable *aux;

    aux = (finishedJobTable *) malloc(sizeof(finishedJobTable));
    if (aux == NULL)
    {
        printf("Error on malloc.");
        exit(1);
    }

    aux->job = finishedJob;
    aux->nextTable = NULL;

    if ((*finishedJobTableHead) == NULL)
    {
        (*finishedJobTableHead) = aux; 
        (*finishedJobTableTail) = aux;       
    }
    else
    {
       (*finishedJobTableTail)->nextTable = aux;
       (*finishedJobTableTail) = aux;
    }    
}

void deleteJobTable(finishedJobTable **finishedJobTableHead, finishedJobTable **finishedJobTableTail)
{
    finishedJobTable *aux;
    while(*finishedJobTableHead != NULL)
    {
        aux = (*finishedJobTableHead);
        (*finishedJobTableHead) = (*finishedJobTableHead)->nextTable;
        free(aux);
    }

    (*finishedJobTableTail) = NULL;
}

void printfJobTable(finishedJobTable *finishedJobTableHead)
{
    while(finishedJobTableHead != NULL)
    {
        printf("---------------------------\n");
        printf("jobOrder: %d\n", finishedJobTableHead->job.jobOrder);
        printf("Seconds: %d\n", finishedJobTableHead->job.seconds);
        printf("StartTime: %ld\n", finishedJobTableHead->job.startTime);
        printf("EndTime: %ld\n", finishedJobTableHead->job.endTime);
        printf("ExeFile: %s\n", finishedJobTableHead->job.exeFile);
        finishedJobTableHead = finishedJobTableHead->nextTable;
    }
    printf("---------------------------\n");
}

void printfJobToExecute(jobQueue *jobHead)
{
    while(jobHead != NULL)
    {
        printf("---------------------------\n");
        printf("Remaining seconds: %d\n", jobHead->remainingSeconds);
        printf("jobOrder: %d\n", jobHead->job.jobOrder);
        printf("Seconds: %d\n", jobHead->job.seconds);
        printf("StartTime: %ld\n", jobHead->job.startTime);
        printf("EndTime: %ld\n", jobHead->job.endTime);
        printf("ExeFile: %s\n", jobHead->job.exeFile);
        jobHead = jobHead->next;
    }
    printf("---------------------------\n");
}