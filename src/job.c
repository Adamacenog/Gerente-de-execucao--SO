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
    jobQueue *aux, *aux2;

    aux = (jobQueue*)malloc(sizeof(jobQueue));    

    if (aux == NULL)
    {
        printf("Error on malloc.");
        exit(1);
    }
    
    aux->remainingSeconds = job.seconds;
    aux->job.jobId = job.jobId;
    aux->job.seconds = job.seconds;
    aux->job.start_time = job.start_time;
    aux->job.end_time = job.end_time;
    strcpy(aux->job.exeFile, job.exeFile);
    aux->next = NULL;
    
    if ((*jobHead) == NULL)
    {
        (*jobHead) = aux;
    }
    else
    {
        aux2 = *jobHead;
        printf("seg\n");
        printf("JobId: %d\n", (*jobHead)->job.jobId);
        printf("next is null? %d\n", aux2->next == NULL);
        while(aux2->next != NULL && aux2->next->remainingSeconds < job.seconds)
        {
            printf("JobId: %d\n", aux2->next->job.jobId);
           aux2 = aux->next;
           printf("JobId: %d\n", aux2->job.jobId);  // It is not printing.... although it should
           printf("next is null? %d\n", aux2->next == NULL);           
        }
        printf("falt\n");
        if(aux2 == (*jobHead))
        {
            if ((*jobHead)->remainingSeconds < job.seconds)
            {
                aux->next = (*jobHead)->next;
                (*jobHead)->next = aux;
            }
            else
            {           
                aux->next = *jobHead;
                (*jobHead) = aux;
            }            
        }
        else
        {
            aux->next = aux2->next;
            aux2->next = aux;
        }        
    }
}

void removeHead(jobQueue **jobHead)
{
    jobQueue *aux;
    aux = (*jobHead);
    (*jobHead) = (*jobHead)->next;
    free(aux);
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
        (*jobHead).remainingSeconds -= amount2Decrease;
        jobHead = jobHead->next;
    }
}

void addToJobTable(finishedJobTable **finishedJobTableHead, job finishedJob)
{
    finishedJobTable *aux, *aux2;

    aux = (finishedJobTable *) malloc(sizeof(finishedJobTable));
    if (aux == NULL)
    {
        printf("Error on malloc.");
        exit(1);
    }

    aux->job = finishedJob;
    aux->next = NULL;

    if ((*finishedJobTableHead) == NULL)
    {
        (*finishedJobTableHead) = aux;        
    }
    else
    {
       /* aux2 =  *finishedJobTableHead;
        while (aux2->next != NULL)
        {
            aux2 = aux2->next;
        }

        aux2->next = aux;*/
    }    
}

void printfJobToExecute(jobQueue *jobHead)
{
    while(jobHead != NULL)
    {
        printf("---------------------------\n");
        printf("Remaining seconds: %d\n", jobHead->remainingSeconds);
        printf("JobId: %d\n", jobHead->job.jobId);
        printf("Seconds: %d\n", jobHead->job.seconds);
        printf("StartTime: %ld\n", jobHead->job.start_time);
        printf("EndTime: %ld\n", jobHead->job.end_time);
        printf("ExeFile: %s\n", jobHead->job.exeFile);
        jobHead = jobHead->next;
    }
    printf("---------------------------\n");
}