/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves. - mat. 14/0135839
Guilherme Lopes
*/

#define MSGSZ 128

// Declare the message structure.
typedef struct msgbuf
{
	long mtype;
	char mtext[MSGSZ];
} msgbuf;

int QueueCreator(void);
void MessageReceive(int, struct msgbuf *, long);
void MessageSend(int, struct msgbuf, size_t);
void QueueDestroy(int msqid);
