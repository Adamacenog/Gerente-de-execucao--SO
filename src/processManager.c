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
  int processManagerId, msqid, topologyId; 
  struct FloodTable floodTable;
  struct NodeJob nodeJobExecution, nodeJobResponses;
  int isExecutingJob = 0, status_ptr;
  pid_t pid;
  char exeFile[50];

  if(argc == 3) 
  { 
    /* Ends if it receives a SIGTERM */
    signal(SIGTERM, endExecution); 
    /* Receive the node id through args  */
    processManagerId = atoi(argv[1]);
    topologyId =  atoi(argv[2]);
    //printf("PROCESS MANAGER ID: %d TOPOLOGY ID: %d\n", processManagerId, topologyId);

    msqid = queueCreator(key); 

    eraseFloodTable(&floodTable);

    while (1)
    {
      if (receiveNodeMessage(msqid, &nodeJobResponses, (processManagerId + 1)))
      {
        if (nodeJobResponses.destination == -1 || nodeJobResponses.destination == processManagerId)
        {
          if (isMessageNew(&nodeJobResponses, &floodTable))
          {
            nodeJobExecution = nodeJobResponses;
            isExecutingJob = 1;                 

            // Checks if fork is created sucessfully
            if ((pid = fork()) < 0)
            {
              printf("Error while creating execution node.\n");
              perror("fork");
              exit(1);
            }
            else
            {
              // Checks whether it is processes father (pid != 0) or child (pid == 0)
              if (pid == 0)
              {
                strcpy(exeFile, "./");
                strcat(exeFile, nodeJobExecution.job.exeFile);
                execl(exeFile, nodeJobExecution.job.exeFile, NULL);            
              }
            }
            
            /* Sets statistics values */
            nodeJobExecution.job.nodePid = getpid();
            nodeJobExecution.job.jobPid = pid;
            nodeJobExecution.job.startTime = time (NULL);
            
            if (nodeJobExecution.destination == -1)
            {
              /* Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, processManagerId,topologyId);
            }
          }
          else
          {
            if (processManagerId == 0)
            {
              /* checks if it is a response from a node */
              if (isResponse(&nodeJobResponses, &floodTable))
              {
                nodeJobResponses.job.nodeId = nodeJobResponses.source;
                
                /* Send response to scheduler (mtype 777) */
                createMessage(msqid, &nodeJobResponses.job, 777);
                printf("Statistics NodeId: %d, JobOrder: %d\n", nodeJobResponses.job.nodeId, nodeJobResponses.job.jobOrder);
              }
            } 
          }
        }
        else
        {
          if (isResponse(&nodeJobResponses, &floodTable))
          {
            /* Floods the message */
            floodNodeMessage(msqid, &nodeJobResponses, processManagerId, topologyId);
          }
        }          
      }

      if (isExecutingJob)
      {
        /* Check if job has finished, with nonblocking wait function */
        /* if it has finished, set statistics and send it to node 0 */
        if (waitpid(pid, &status_ptr, WUNTRACED) == pid)
        {
          if (WIFEXITED(status_ptr))
          {
            nodeJobExecution.job.endTime = time (NULL);
            nodeJobExecution.source = processManagerId;
            nodeJobExecution.destination = 0;
            isExecutingJob = 0;

            if (processManagerId != 0)
            {
              /* Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, processManagerId, topologyId);
            }
            else
            {
              nodeJobResponses.job.nodeId = 0;
              
              /* Sends zero response to scheduler (mtype 777) */
              createMessage(msqid, &nodeJobExecution.job, 777);
              printf("Statistics NodeId: %d, JobOrder: %d\n", nodeJobExecution.job.nodeId, nodeJobExecution.job.jobOrder);
            }             
          }
          else
          {
            printf("Error: Job %d in Node %d returned %d\n", nodeJobExecution.job.jobOrder, processManagerId, status_ptr);
          }       
        }
        else
        {
          printf("Execution Error.\n");
          perror("waitpid");
        }      
      }
      
      if (processManagerId == 0)
      {
        /* Checks if threre are any new message from scheduler */
        getSchedulerMsg(msqid);
      }
    }
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
  exit(0);
}

/* convertToBinary function converts a string to binary */
void convertToBinary(int *dest, int source)
{
  int i, k;
  
  memset(dest,0,4*sizeof(int));
 
  for (i = 0; i < 4; i++)
  {
    k = source >> i;
 
    if (k & 1)
      dest[i] = 1;
    else
      dest[i] = 0;
  }
}

void convertFromBinary2Int(int *dest, int *source)
{
  *dest = source[0] + source[1]*2 + source[2]*4 + source[3]*8;
}

int isMessageNew(nodeJob *nodeJob, floodTable *floodTable)
{              
  if (floodTable->uniqueId == nodeJob->job.jobOrder && floodTable->wasExecuted == 0)
  {
    floodTable->wasExecuted = 1;
    return 1;
  } 

  if (floodTable->uniqueId != nodeJob->job.jobOrder)
  {
    eraseFloodTable(floodTable);
    floodTable->uniqueId = nodeJob->job.jobOrder;
    floodTable->wasExecuted = 1;
    return 1;
  }

  return 0;
}

int isResponse(nodeJob *nodeJob,floodTable *floodTable)
{
  if (floodTable->nodesResponse[nodeJob->source] == 0)
  {
    if (floodTable->uniqueId != nodeJob->job.jobOrder)
    {
      eraseFloodTable(floodTable);
      floodTable->uniqueId = nodeJob->job.jobOrder;
    }

    floodTable->nodesResponse[nodeJob->source] = 1;
    
    return 1;
  }

  return 0;
}

int receiveNodeMessage(int msqid, nodeJob *nodeJob, int nodeId)
{
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

    convertBuf2Job(bufReceive.mtext, &nodeJob->job); 

    return 1;
  }

  return 0;
}

void eraseFloodTable(floodTable *floodTable)
{
  floodTable->wasExecuted = 0;
  floodTable->uniqueId = -1;

  for (int i = 0; i < 16; i++)
    floodTable->nodesResponse[i] = 0;
}

void getSchedulerMsg(int msqid)
{
  struct Job job;
  struct NodeJob nodeJob;
  

  /* Checks if threre are any new message from scheduler (mtype = 555) */
  if (receiveMessage(msqid, &job, 555))
  {
    /* Node 0 receives the message, and converts it to the nodes message 'language' */
    
    nodeJob.destination = -1;
    nodeJob.source = 0;
    nodeJob.job.nodeId = 0;
    nodeJob.job = job;
    
    sendNodeMessage(msqid, &nodeJob, 1);
  }
}

void sendNodeMessage(int msqid, struct NodeJob *nodejob, long mtype)
{
  struct msgbuf buf; 
  char auxString[10];

  memset(buf.mtext,0,MSGSZ);
  memset(auxString,0,10);

  /* sets destination */
  sprintf(auxString, "%d", nodejob->destination);
  strcat(buf.mtext, auxString);
  strcat(buf.mtext, "|");
  memset(auxString,0,10);

  /* sets source */
  sprintf(auxString, "%d", nodejob->source);
  strcat(buf.mtext, auxString);
  strcat(buf.mtext, "|");

  convertJob2Buf(&nodejob->job, buf.mtext);

  /* Sends the converted message to node 0 */
  buf.mtype = mtype;

  messageSend(msqid, buf, (strlen(buf.mtext) + 1));
}

void floodNodeMessage(int msqid, nodeJob *nodeJob, int processManagerId, int topology)
{
  int whoSent = nodeJob->job.nodeId, binaryAddr[4], i, adjacentNode[4], sendValue;
  struct msgbuf buf;

  /* Sets the nodeId of the node that is sending the message */
  nodeJob->job.nodeId = processManagerId;

  switch (topology)
  {
    /* Hypercube */
    case 1:
      convertToBinary(binaryAddr, processManagerId);

      for (i = 0; i < 4; i++)
      {
        if ( binaryAddr[i] == 1)
          binaryAddr[i] = 0;
        else
          binaryAddr[i] = 1;
        
        convertFromBinary2Int(&sendValue, binaryAddr);
        
        if (sendValue != whoSent && sendValue != processManagerId)
        {
          /* Send the message with mtype sendValue */
          sendNodeMessage(msqid, nodeJob, (sendValue + 1));
        }

        if ( binaryAddr[i] == 1)
          binaryAddr[i] = 0;
        else
          binaryAddr[i] = 1;       
      }
      break;
    
    /* Torus */
    case 2:
    /* code */
      break;
    
    /* Fat_tree */
    case 3:
    /* code */
      break;
  
    default:
      printf("Error: Invalid Topology!\n");
      exit(1);
      break;
  }
}