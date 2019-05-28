/*
Propriedade de:
Alex Nascimento. - mat. 15/0115474
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes. - mat. 15/0128215
*/

#ifndef _Primary_libraries
  #define _Primary_libraries
    #include <stdio.h>
    #include <ctype.h>
    #include <string.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <unistd.h>
    #include <time.h>
    #include <signal.h>
#endif

#ifndef _Queue_library
  #define _Queue_library
    #include "messageQueue.h"
#endif

#ifndef _Job_Queue
  #define _Job_Queue
    #include "job.h"
#endif

typedef struct FloodTable
{
  int wasExecuted;    // is 0 when uniqueId was filled by a nodes response, 1 when uniqueId was filled by a job execution
  int uniqueId;
  int nodesResponse[16];
  struct FloodTable *next;
} floodTable;

typedef struct NodeJob
{
  int destination;
  int source;
  struct Job job;
} nodeJob;

void endExecution(int);
void convertToBinary(char *, int);
void addToFloodTableUniqueId(floodTable **, struct NodeJob *);              // Receives the floodTable head and uniqueId
void addToFloodTableNodesResponse(floodTable **, struct NodeJob *);    // Receives the floodTable head, node # and uniqueId
int isInFloodTable(floodTable *, int, floodTable **);
void deleteFloodTable(floodTable **);
void sendNodeMessage(int, struct NodeJob *, int);
int receiveNodeMessage(int, struct NodeJob *, int);
int isMessageNew(floodTable *, struct NodeJob *);
int isResponse(floodTable *, struct NodeJob *);
