/*
Propriedade de:
Alex Nascimento
Andre Garrido Damaceno.- mat. 15/0117531
Danillo Neves
Guilherme Lopes
*/

// Declare the message structure.
typedef struct msgbuf
{
	long mtype;
	char mtext[MSGSZ];
} msgbuf;

int QueueCreator(void);
char * MessageReceive(int, msgbuf, long);
void MessageSend(int, msgbuf);
