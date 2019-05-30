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
    printf("PROCESS MANAGER ID: %d TOPOLOGY ID: %d\n", processManagerId, topologyId);

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
            printf("Node: %d is Executing!\n", processManagerId);
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
            nodeJobExecution.job.nodeId = processManagerId;
            nodeJobExecution.job.nodePid = getpid();
            nodeJobExecution.job.jobPid = pid;
            nodeJobExecution.job.startTime = time (NULL);
            
            if (nodeJobExecution.destination == -1)
            {
              /* Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, topologyId);
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
              }
            } 
          }
        }
        else
        {
          if (isResponse(&nodeJobResponses, &floodTable))
          {
            nodeJobResponses.job.nodeId = processManagerId;

            /* Floods the message */
            floodNodeMessage(msqid, &nodeJobResponses, topologyId);
          }
        }          
          // Check message (nodeJob), if new (checking the nodeJob->job.jobOrder, using function
          // 'isNewMessage'), put on job queue (TO DO) and flood
          // if message is repeated, discart it completely (don't flood!!).

          // Each node can take a new job after the previous job has finished the execution or
          // if the node is free (not executing any job)            
      }

      if (isExecutingJob)
      {
        /* Check if job has finished, with nonblocking wait function */
        /* if it has finished, set statistics and send it to node 0 */
        if (waitpid(pid, &status_ptr, WUNTRACED) == pid)
        {
          if (&status_ptr != NULL)
          {
            if (WIFEXITED(status_ptr))
            {
              nodeJobExecution.job.endTime = time (NULL);
              nodeJobExecution.source = processManagerId;
              nodeJobExecution.destination = 0;
              isExecutingJob = 0;

              /* REMOVE!!! */
              char buffer[200];
              struct tm* tm_info;
              tm_info = localtime(&nodeJobExecution.job.startTime);
              printf("Tempo de inicio: ");
              strftime(buffer, 200, "%Y-%m-%d %H:%M:%S", tm_info);
              puts(buffer);
              tm_info = localtime(&nodeJobExecution.job.endTime);
              printf("Tempo de fim: ");
              strftime(buffer, 200, "%Y-%m-%d %H:%M:%S", tm_info);
              puts(buffer);
              printf("\n");
              /* REMOVE!!! */
              
              /* Floods the message */
              floodNodeMessage(msqid, &nodeJobExecution, topologyId);
            }
            else
            {
              printf("Error: Job %d in Node %d returned %d\n", nodeJobExecution.job.jobOrder, processManagerId, status_ptr);
            }       
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

/* convertToBinary function converts a string to binary */
void convertToBinary(char *dest, int source)
{
  int i, k;
  
  memset(dest,0,strlen(dest));
 
  for (i = 3; i >= 0; i--)
  {
    k = source >> i;
 
    if (k & 1)
      strcat(dest, "1");
    else
      strcat(dest, "0");
  }
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
  struct msgbuf buf; 
  struct Job job;
  char auxString[10];

  /* Checks if threre are any new message from scheduler (mtype = 555) */
  if (receiveMessage(msqid, &job, 555))
  {
    printf("Received from scheduler!!\n");
    /* Node 0 receives the message, and converts it to the nodes message 'language' */
   
    memset(buf.mtext,0,MSGSZ);
    memset(auxString,0,10);

    /* sets destination */
    sprintf(auxString, "%d", -1);
    strcat(buf.mtext, auxString);
    strcat(buf.mtext, "|");
    memset(auxString,0,10);

    /* sets source */
    sprintf(auxString, "%d", 0);
    strcat(buf.mtext, auxString);
    strcat(buf.mtext, "|");
  
    convertJob2Buf(&job, buf.mtext);

    /* Sends the converted message to node 0 */
    buf.mtype = 1;

    messageSend(msqid, buf, (strlen(buf.mtext) + 1));
  }
}

void floodNodeMessage(int msqid, nodeJob *nodeJob, int topology)
{
  switch (topology)
  {
    /* Hypercube */
    case 1:
    /* code */
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