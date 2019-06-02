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
        printf("Job order: %d\n", jobTableHead->job.jobOrder);
        printf("Requesting process pid: %d\n", jobTableHead->job.delayedPid);
        printf("Total seconds waited: %d\n", jobTableHead->job.seconds);       
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
        printf("Job order: %d\n", jobHead->job.jobOrder);
        printf("Requesting process pid: %d\n", jobHead->job.delayedPid);
        printf("Remaining seconds to wait: %d\n", jobHead->remainingSeconds);
        printf("Total seconds requested to wait: %d\n", jobHead->job.seconds);
        printf("ExeFile: %s\n", jobHead->job.exeFile);
        jobHead = jobHead->next;
    }
    printf("---------------------------\n");
}

void printfJobStatistics(jobTable *jobTableHead, jobTable *jobTableTail)
{
    jobTable *aux;
    aux = jobTableHead;
    int hasEnded = 1, printMessage = 1;

    while (aux != NULL)
    {
        if (aux->job.endTime == -1)
            hasEnded = 0;

        aux = aux->nextTable;
    }

    printf("---------------------------------------------------------------------------------------------------------------------------------\n");
    printf("|Job: %d | Requesting process pid: %d | File: %s | Delay: %d seconds | makespan:", jobTableHead->job.jobOrder, jobTableHead->job.delayedPid, jobTableHead->job.exeFile, jobTableHead->job.seconds);

    if (hasEnded == 1)
    {
        printf(" %ld seconds | Fastest node: %d | Slowest node: %d\n",(jobTableTail->job.endTime - jobTableHead->job.startTime), jobTableHead->job.nodeId, jobTableTail->job.nodeId);
        printf("|Minimum startTime: %s", ctime(&jobTableHead->job.startTime));
        printf("|Maximum endTime: %s", ctime(&jobTableTail->job.endTime));
    }
    else
    {        
        aux = jobTableHead;
        while (aux != NULL)
        {
            if (aux->job.endTime != -1 && printMessage)
            {
                printf(" - not every node ended the execution!\n");
                printf("\nNodes that ended the execution:\n");
                printMessage = 0;
            }

            if (aux->job.endTime != -1)
            {
                printf("\n|Node: %d\n", aux->job.nodeId);
                printf("|Job: %d | Requesting process pid: %d | File: %s | Delay: %d seconds | makespan: %ld seconds\n", aux->job.jobOrder, aux->job.delayedPid, aux->job.exeFile, aux->job.seconds, (aux->job.endTime - aux->job.startTime));
                printf("|StartTime: %s", ctime(&aux->job.startTime));
                printf("|EndTime: %s", ctime(&aux->job.endTime));
            }

            aux = aux->nextTable;
        }

        if (printMessage)
        {
            printf(" - no node has finished running!\n");
        }
    }   

    printf("---------------------------------------------------------------------------------------------------------------------------------\n");
}

void removeJobHead(jobTable **jobTableHead, jobTable **jobTableTail)
{
    jobTable *aux;
    if ((*jobTableHead) != NULL)
    {
        aux = (*jobTableHead);
        (*jobTableHead) = (*jobTableHead)->nextTable;
        free(aux);

        if ((*jobTableHead) == NULL)
        {
            (*jobTableTail) = NULL;
        }
    }
}