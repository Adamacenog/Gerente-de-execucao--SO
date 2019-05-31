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

    /* Receive the node id and topology id through args  */
    processManagerId = atoi(argv[1]);
    topologyId =  atoi(argv[2]);
    
    /* Gets the queue msqid according to the key */
    msqid = queueCreator(key); 

    /* Sets the flood table to its start values */
    eraseFloodTable(&floodTable);

    while (1)
    {
      /* Receives a message sent from a node, using the topology route */
      if (receiveNodeMessage(msqid, &nodeJobResponses, (processManagerId + 1)))
      {
        /* Checks if message is a soft kill sent from scheduler */
        if (nodeJobResponses.job.jobOrder == -1 && floodTable.terminate == 0)
        {
          /* Sets terminate */
          floodTable.terminate = 1;

          if (isExecutingJob)
          {
            // Kill child process
            if (kill(pid,SIGTERM) == -1)
            {
              printf("Error while sending signal.\n");
              perror("kill");
            }

            if (waitpid(pid, &status_ptr, WUNTRACED) == pid)
            {
              if (!WIFEXITED(status_ptr))
              {
                printf("Error, node child (program being executed) did not finish correctly!\n");
              }               
            }
            else
            {
              printf("Execution Error.\n");
              perror("waitpid");
            }

            /* Sets the statistics final values */
            nodeJobExecution.job.endTime = -1;
            nodeJobExecution.source = processManagerId;
            nodeJobExecution.destination = 0;

            /* Sets the node as free */
            isExecutingJob = 0;

            /* Checks if execution node was node 0 */
            if (processManagerId != 0)
            {
              /* If it is not node 0, Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, processManagerId, topologyId);
            }
            else
            {
              /* if it is node 0, it sends the statistics directly to scheduler */
              nodeJobExecution.job.nodeId = 0;
              
              /* Sends zero response to scheduler (mtype 777) */
              createMessage(msqid, &nodeJobExecution.job, 777);
            }
          }

          /* Floods the message */
          floodNodeMessage(msqid, &nodeJobResponses, processManagerId,topologyId);
        }
        else
        {
          /* Checks if the message is to everyone (-1) or to the current node id */
          if (nodeJobResponses.job.jobOrder != -1 && (nodeJobResponses.destination == -1 || nodeJobResponses.destination == processManagerId))
          {
            /* Checks if the message is new. If it is, it executes it. If its not, it ignores it */
            if (isExecutingJob == 0 && isMessageNew(&nodeJobResponses, &floodTable))
            {
              /* Copies the response struct to the execution struct, to save the execution values */
              nodeJobExecution = nodeJobResponses;

              /* Set the node as busy */
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
              
              /* Checks if message was meant to everyone */
              if (nodeJobExecution.destination == -1)
              {
                /* Floods the message */
                floodNodeMessage(msqid, &nodeJobExecution, processManagerId,topologyId);
              }
            }
            else
            {
              /* If it is not a new message, then the node 0 needs to check if it is a response from another node sending its statistics */
              if (processManagerId == 0)
              {
                /* checks if it is a response from a node */
                if (isResponse(&nodeJobResponses, &floodTable))
                {
                  /* If it is a response, restores the original node id (using the message source) */
                  nodeJobResponses.job.nodeId = nodeJobResponses.source;
                  
                  /* Send response to scheduler (mtype 777) */
                  createMessage(msqid, &nodeJobResponses.job, 777);
                }
              } 
            }
          }
          else
          {
            if (nodeJobResponses.job.jobOrder != -1)
            {
              /* If the message is not to everyone and the destination is not the current node, checks if it is a statistics response from a node */
              if (isResponse(&nodeJobResponses, &floodTable))
              {
                /* Floods the message */
                floodNodeMessage(msqid, &nodeJobResponses, processManagerId, topologyId);
              }
            }
          }
        }          
      }

      if (isExecutingJob)
      {
        /* Check if job has finished executing, with nonblocking wait function */
        /* if it has finished, set statistics and send it to node 0 */
        if (waitpid(pid, &status_ptr, WNOHANG) == pid)
        {
          /* Macro to analyse the return value from the child execution */
          if (WIFEXITED(status_ptr))
          {
            /* Sets the statistics final values */
            nodeJobExecution.job.endTime = time (NULL);
            nodeJobExecution.source = processManagerId;
            nodeJobExecution.destination = 0;

            /* Sets the node as free */
            isExecutingJob = 0;

            /* Checks if execution node was node 0 */
            if (processManagerId != 0)
            {
              /* If it is not node 0, Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, processManagerId, topologyId);
            }
            else
            {
              /* if it is node 0, it sends the statistics directly to scheduler */
              nodeJobResponses.job.nodeId = 0;
              
              /* Sends zero response to scheduler (mtype 777) */
              createMessage(msqid, &nodeJobExecution.job, 777);
            }             
          }
          else
          {
            printf("Error: Job %d in Node %d returned %d\n", nodeJobExecution.job.jobOrder, processManagerId, status_ptr);
          }       
        }
        else
        {
          if (errno != 42 && errno != 0)
          {
            printf("Execution Error.\n");
            perror("waitpid");
          }
        }      
      }
      
      if (processManagerId == 0)
      {
        /* Checks if there are any new message from scheduler */
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

/* convertToBinary function converts a int to a binary int array */
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

/* convertFromBinary2Int function converts a binary int array to a int */
void convertFromBinary2Int(int *dest, int *source)
{
  *dest = source[0] + source[1]*2 + source[2]*4 + source[3]*8;
}

/* isMessageNew checks if node has received a new message */
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

/* isResponse checks if the message that node has received is anothers node response */
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

/* receiveNodeMessage receives a node message and put it in nodeJob struct, returning 1 when there is a new message and 0 when there is no new message */
int receiveNodeMessage(int msqid, nodeJob *nodeJob, int nodeId)
{
  struct msgbuf bufReceive;
  char auxString[AUXSZ], pattern[2] = "|";

  /* Receives a msg with mtype # */
  if (messageReceive(msqid, &bufReceive, nodeId, 0))
  {
    /* Cuts the string with the pattern to be parsed and Sets the job values */
    memset(auxString,0,AUXSZ);
    copyNremoveByPattern(auxString, AUXSZ, bufReceive.mtext, MSGSZ, *pattern);
    nodeJob->destination = atoi(auxString);

    memset(auxString,0,AUXSZ);
    copyNremoveByPattern(auxString, AUXSZ, bufReceive.mtext, MSGSZ, *pattern);
    nodeJob->source = atoi(auxString);

    convertBuf2Job(bufReceive.mtext, &nodeJob->job); 

    return 1;
  }

  return 0;
}

/* sets the flood table to its default values */
void eraseFloodTable(floodTable *floodTable)
{
  floodTable->wasExecuted = 0;
  floodTable->uniqueId = -1;
  floodTable->terminate = 0;

  for (int i = 0; i < 16; i++)
    floodTable->nodesResponse[i] = 0;
}

/* Gets the message sent from scheduler - Only node 0 can call this function - */
void getSchedulerMsg(int msqid)
{
  struct Job job;
  struct NodeJob nodeJob;
  

  /* Checks if threre are any new message from scheduler (mtype = 555) */
  if (receiveMessage(msqid, &job, 555))
  {
    cleanAllRemainingMessages(msqid);

    /* Node 0 receives the message, and converts it to the nodes message 'language' */
    
    nodeJob.destination = -1;
    nodeJob.source = 0;
    nodeJob.job.nodeId = 0;
    nodeJob.job = job;
    
    sendNodeMessage(msqid, &nodeJob, 1);
  }
}

void cleanAllRemainingMessages(int msqid)
{
  int hasMessage = 1, i, aux = 0;
  struct NodeJob nodeJobResponses;

  while(hasMessage == 1)
  {
    for (i = 1; i < 17; i++)
    {
      hasMessage = receiveNodeMessage(msqid, &nodeJobResponses, i);

      if (hasMessage == 0)
        aux++;
    }

    if (aux == 16)
      hasMessage = 0;
    else
      hasMessage = 1; 

    aux = 0;   
  }
}

/* Sends the message 'nodeJob' from node source to node destination */
void sendNodeMessage(int msqid, struct NodeJob *nodejob, long mtype)
{
  struct msgbuf buf; 
  char auxString[AUXSZ];

  memset(buf.mtext,0,MSGSZ);
  memset(auxString,0,AUXSZ);

  /* sets destination */
  sprintf(auxString, "%d", nodejob->destination);
  strcat(buf.mtext, auxString);
  strcat(buf.mtext, "|");
  memset(auxString,0,AUXSZ);

  /* sets source */
  sprintf(auxString, "%d", nodejob->source);
  strcat(buf.mtext, auxString);
  strcat(buf.mtext, "|");

  convertJob2Buf(&nodejob->job, buf.mtext);

  /* Sends the converted message to node 0 */
  buf.mtype = mtype;

  messageSend(msqid, buf, (strlen(buf.mtext) + 1));
}

/* Floods the message from one node to all nodes, in a clever way, according to the topology selected by the user */
void floodNodeMessage(int msqid, nodeJob *nodeJob, int processManagerId, int topology)
{
  int whoSent = nodeJob->job.nodeId, binaryAddr[4], i, adjacentNode[4], sendValue, neighbours[4] = {0,0,0,0};
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
      switch (processManagerId)
      {
        case 0:
          assignVector(neighbours, 1, 3, 4, 12);
          break;
          
        case 1:
          assignVector(neighbours, 0, 2, 5, 13);
          break;
        
        case 2:
          assignVector(neighbours, 1, 3, 6, 14);
          break;
      
        case 3:
          assignVector(neighbours, 0, 2, 7, 15);
          break;

        case 4:
          assignVector(neighbours, 5, 7, 8, 12);
          break;

        case 5:
          assignVector(neighbours, 1, 4, 6, 9);
          break;

        case 6:
          assignVector(neighbours, 2, 5, 7, 10);
          break;

        case 7:
          assignVector(neighbours, 3, 4, 6, 11);
          break;

        case 8:
          assignVector(neighbours, 4, 9, 11, 12);
          break;

        case 9:
          assignVector(neighbours, 5, 8, 10, 13);
          break;

        case 10:
          assignVector(neighbours, 6, 9, 11, 14);
          break;

        case 11:
          assignVector(neighbours, 7, 8, 10, 15);
          break;

        case 12:
          assignVector(neighbours, 0, 8, 13, 15);
          break;

        case 13:
          assignVector(neighbours, 1, 9, 12, 14);
          break;

        case 14:
          assignVector(neighbours, 2, 10, 13, 15);
          break;

        case 15:
          assignVector(neighbours, 3, 11, 12, 14);
          break;

        default:
          printf("Error: Unknown behaviour ¯\\_(ツ)_/¯!\n");
          break;
      }     

      for (i = 0; i < 4; i++)
      {
        if (whoSent != neighbours[i])
        {
          /* Send the message with mtype neighbours[i] */
          sendNodeMessage(msqid, nodeJob, (neighbours[i] + 1));
        }
      }
          
      break;
    
    /* Fat_tree */
    case 3:
      switch (processManagerId)
      {
        case 0:
          assignVector(neighbours, 1, 2, -1, -1);
          break;
          
        case 1:
          assignVector(neighbours, 0, 3, 4, -1);
          break;
        
        case 2:
          assignVector(neighbours, 0, 5, 6, -1);
          break;
      
        case 3:
          assignVector(neighbours, 1, 7, 8, -1);
          break;

        case 4:
          assignVector(neighbours, 1, 9, 10, -1);
          break;

        case 5:
          assignVector(neighbours, 2, 11, 12, -1);
          break;

        case 6:
          assignVector(neighbours, 2, 13, 14, -1);
          break;

        case 7:
          assignVector(neighbours, 3, -1, -1, -1);
          break;

        case 8:
          assignVector(neighbours, 3, -1, -1, -1);
          break;

        case 9:
          assignVector(neighbours, 4, -1, -1, -1);
          break;

        case 10:
          assignVector(neighbours, 4, -1, -1, -1);
          break;

        case 11:
          assignVector(neighbours, 5, -1, -1, -1);
          break;

        case 12:
          assignVector(neighbours, 5, -1, -1, -1);
          break;

        case 13:
          assignVector(neighbours, 6, -1, -1, -1);
          break;

        case 14:
          assignVector(neighbours, 6, -1, -1, -1);
          break;

        default:
          printf("Error: Unknown behaviour ¯\\_(ツ)_/¯!\n");
          break;
      }    

      for (i = 0; i < 3; i++)
      {
        if (whoSent != neighbours[i] && neighbours[i] != -1)
        {
          /* Send the message with mtype neighbours[i] */
          sendNodeMessage(msqid, nodeJob, (neighbours[i] + 1));
        }
      }
  }
}

void assignVector(int *vector, int a, int b, int c, int d)
{
  vector[0] = a;
  vector[1] = b;
  vector[2] = c;
  vector[3] = d;
}