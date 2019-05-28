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

int main(int argc, char const *argv[]) 
{  
  key_t key = 7869; 
  struct msgbuf pmBuffer; 
  int processManagerId, msqid, topologyId; 
  struct NodeJob *nodeJob;
  char execFile[10], *seconds, pattern[2] = "|"; 
    
  if(argc == 3) 
  { 
      /* Ends if it receives a SIGTERM */
      signal(SIGTERM, endExecution); 
      /* Receive the node id through args  */
      processManagerId = atoi(argv[1]);
      topologyId =  atoi(argv[2]);
      printf("PROCESS MANAGER ID: %d\nTOPOLOGY ID: %d\n", processManagerId, topologyId);
  
      msqid = queueCreator(key); 

      while (1)
      {
        if (receiveNodeMessage(msqid, nodeJob, (processManagerId + 1)))
        {
            // Check message (nodeJob), if new (checking the nodeJob->job.jobOrder, using function
            // 'isNewMessage'), put on job queue (TO DO) and flood
            // if message is repeated, discart it completely (don't flood!!).

            // Each node can take a new job after the previous job has finished the execution or
            // if the node is free (not executing any job)            
        }
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
  else 
  { 
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

int isInFloodTable(floodTable *floodTableHead, int uniqueId, floodTable **found)
{
  while (floodTableHead != NULL)
  {
    if (floodTableHead->uniqueId == uniqueId)
    {
      (*found) = floodTableHead;

      return 1;
    }

    floodTableHead = floodTableHead->next;
  }

  (*found) = NULL;
  return 0;
}

void addToFloodTableUniqueId(floodTable **floodTableHead, struct NodeJob *nodeJob)
{
  floodTable *aux, *aux2;

  if (isInFloodTable((*floodTableHead), nodeJob->job.jobOrder, &aux))
  {
    aux->wasExecuted = 1;
  }
  else
  {
    aux = (floodTable *) malloc(sizeof(floodTable));
    if (aux == NULL)
    {
      printf("Error on malloc.");
      exit(1);
    }

    aux->uniqueId = nodeJob->job.jobOrder;
    aux->wasExecuted = 1;
    aux->next = NULL;

    if ((*floodTableHead) == NULL)
    {
      (*floodTableHead) = aux;
    }
    else
    {
      aux2 = (*floodTableHead);

      while (aux2->next != NULL)
        aux2 = aux2->next;

      aux2->next = aux;
    }  
  }  
}

void addToFloodTableNodesResponse(floodTable **floodTableHead, struct NodeJob *nodeJob)
{
  floodTable *aux, *aux2;

  if (isInFloodTable((*floodTableHead), nodeJob->job.jobOrder, &aux))
  {
    aux->nodesResponse[nodeJob->source] = 1;
  }
  else
  {
    aux = (floodTable *) malloc(sizeof(floodTable));
    if (aux == NULL)
    {
      printf("Error on malloc.");
      exit(1);
    }

    aux->uniqueId = nodeJob->job.jobOrder;
    aux->wasExecuted = 0;
    aux->nodesResponse[nodeJob->source] = 1;
    aux->next = NULL;

    if ((*floodTableHead) == NULL)
    {
      (*floodTableHead) = aux;
    }
    else
    {
      aux2 = (*floodTableHead);

      while (aux2->next != NULL)
        aux2 = aux2->next;

      aux2->next = aux;
    }  
  }  
}

void deleteFloodTable(floodTable **floodTableHead)
{
  floodTable *aux;

  while ((*floodTableHead) != NULL)
  {
    aux = (*floodTableHead);
    (*floodTableHead) = (*floodTableHead)->next;
    free(aux);
  }
}

int isMessageNew(floodTable *floodTableHead, struct NodeJob *nodeJob)
{
  floodTable *found = NULL;
  if (isInFloodTable(floodTableHead, nodeJob->job.jobOrder, &found))
  {
    if (found->wasExecuted == 0)
      return 1;
    else
      return 0;
  }

  return 1;
}

int isResponse(floodTable *floodTableHead, struct NodeJob *nodeJob)
{
  floodTable *found = NULL;
  if (isInFloodTable(floodTableHead, nodeJob->job.jobOrder, &found))
  {
    if (found->nodesResponse[nodeJob->source] == 0)
      return 1;
    else
      return 0;
  }

  return 1;
}

int receiveNodeMessage(int msqid, struct NodeJob *nodeJob, int nodeId)
{
  struct Job *job;
  struct msgbuf bufReceive;
  char auxString[50], pattern[2] = "|";

  /* Receives a msg with mtype # */
  if (messageReceive(msqid, &bufReceive, nodeId, 0))
  {
    /* Cuts the string with the pattern to be parsed and Sets the job values */
    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive.mtext, 500, *pattern);
    nodeJob->destination = atoi(auxString);

    memset(auxString,0,50);
    copyNremoveByPattern(auxString, 50, bufReceive.mtext, 500, *pattern);
    nodeJob->source = atoi(auxString);

    convertBuf2Job(bufReceive.mtext, job);  

    nodeJob->job.nodeId = job->nodeId;
    nodeJob->job.delayedPid = job->delayedPid;
    nodeJob->job.jobPid = job->jobPid;
    nodeJob->job.jobOrder = job->jobOrder;
    nodeJob->job.seconds = job->seconds;
    nodeJob->job.startTime = job->startTime;
    nodeJob->job.endTime = job->endTime;
    strcpy(nodeJob->job.exeFile, job->exeFile);

    return 1;
  }

  return 0;
}

void sendNodeMessage(int msqid, struct NodeJob *nodeJob, int topology)
{
  
}