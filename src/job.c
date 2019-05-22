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
    
    (*aux).remainingSeconds = job.seconds;
    (*aux).job = job;
    (*aux).next = NULL;
    
    if ((*jobHead) == NULL)
    {
        (*jobHead) = aux;
    }
    else
    {
        aux2 = *jobHead;

        while(aux2->next != NULL && (*aux2->next).remainingSeconds < job.seconds)
        {
           aux2 = aux->next;
        }

        if(aux2 == (*jobHead))
        {
            if ((**jobHead).remainingSeconds < job.seconds)
            {
                aux->next = (*jobHead)->next;
                (*jobHead)->next = aux;
            }
            else
            {           
                aux->next = (*jobHead);
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