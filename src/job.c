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
    jobQueue *aux;
    
    if ((*jobHead) == NULL)
    {
        (*jobHead) = (jobQueue*)malloc(sizeof(jobQueue));    

        if ((*jobHead) == NULL)
        {
            printf("Error on malloc.");
            exit(1);
        }
        
        (**jobHead).remainingSeconds = job.seconds;
        (**jobHead).job = job;
        (**jobHead).next = NULL;
    }
    else
    {
        aux = *jobHead;

        while(aux->next != NULL && (*aux).remainingSeconds < job.seconds)
        {
           aux = aux->next;
        }

        aux->next = (jobQueue*)malloc(sizeof(jobQueue));    
        aux = aux->next;

        if (aux == NULL)
        {
            printf("Error on malloc.");
            exit(1);
        }
        
        (*aux).remainingSeconds = job.seconds;
        (*aux).job = job;
        (*aux).next = NULL;
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