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
    aux->job.nodeId = job.nodeId;
    aux->job.nodePid = job.nodePid;
    aux->job.delayedPid = job.delayedPid;
    aux->job.jobPid = job.jobPid;
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
        if ((*jobHead)->remainingSeconds > job.seconds)
        {
            aux->next = (*jobHead);
            (*jobHead) = aux;
        }
        else
        {
            aux2 = (*jobHead);
            aux3 = aux2;

            while(aux2->next != NULL && aux2->remainingSeconds <= job.seconds)
            {
                aux3 = aux2;
                aux2 = aux2->next;
            }

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

void addToJobTable(jobTable **jobTableHead, jobTable **jobTableTail, job finishedJob)
{
    jobTable *aux;

    aux = (jobTable *) malloc(sizeof(jobTable));
    if (aux == NULL)
    {
        printf("Error on malloc.");
        exit(1);
    }

    aux->job = finishedJob;
    aux->nextTable = NULL;

    if ((*jobTableHead) == NULL)
    {
        (*jobTableHead) = aux; 
        (*jobTableTail) = aux;       
    }
    else
    {
       (*jobTableTail)->nextTable = aux;
       (*jobTableTail) = aux;
    }    
}

void deleteJobTable(jobTable **jobTableHead, jobTable **jobTableTail)
{
    jobTable *aux;
    while(*jobTableHead != NULL)
    {
        aux = (*jobTableHead);
        (*jobTableHead) = (*jobTableHead)->nextTable;
        free(aux);
    }

    (*jobTableTail) = NULL;
}

void printfJobTable(jobTable *jobTableHead)
{
    while(jobTableHead != NULL)
    {
        printf("---------------------------\n");
        printf("Node id: %d\n", jobTableHead->job.nodeId);
        printf("Node pid: %d\n", jobTableHead->job.nodePid);
        printf("Delayed pid: %d\n", jobTableHead->job.delayedPid);
        printf("Job pid: %d\n", jobTableHead->job.jobPid);
        printf("Job order: %d\n", jobTableHead->job.jobOrder);
        printf("Total seconds waited: %d\n", jobTableHead->job.seconds);
        printf("StartTime: %s", ctime (&jobTableHead->job.startTime));
        
        if (jobTableHead->job.endTime != -1)
        {
            printf("EndTime: %s", ctime (&jobTableHead->job.endTime));
        }
        else
        {
            printf("Process did not finish execution, EndTime is not set!\n");
        }       
        
        printf("ExeFile: %s\n", jobTableHead->job.exeFile);
        jobTableHead = jobTableHead->nextTable;
    }
    printf("---------------------------\n");
}

void printfJobToExecute(jobQueue *jobHead)
{
    while(jobHead != NULL)
    {
        printf("---------------------------\n");
        printf("Remaining seconds: %d\n", jobHead->remainingSeconds);
        printf("Delayed pid: %d\n", jobHead->job.delayedPid);
        printf("Job order: %d\n", jobHead->job.jobOrder);
        printf("Total seconds to wait: %d\n", jobHead->job.seconds);
        printf("ExeFile: %s\n", jobHead->job.exeFile);
        jobHead = jobHead->next;
    }
    printf("---------------------------\n");
}

void removeJobHead(jobTable **jobTableHead)
{
    jobTable *aux;
    if (jobTableHead != NULL)
    {
        aux = (*jobTableHead);
        (*jobTableHead) = (*jobTableHead)->nextTable;
        free(aux);
    }
}