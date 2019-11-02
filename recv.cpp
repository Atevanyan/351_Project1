#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "msg.h"    /* For the message struct */
using namespace std;

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	key_t key;
	key = 	ftok("keyfile.txt",'a');
	if (key < 0)
 {
			 perror("ERROR GETTING KEY");
			 exit(1);
 }
	
		shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666|IPC_CREAT);
		if( shmid == -1)
		{
			perror("ERROR GETTING MEMORY SEGMENT");
			exit(1);
		}
	
		sharedMemPtr = shmat(shmid, (void*)0 ,0);
	
		msqid = msgget(key, IPC_CREAT|0666);
	
		if (msqid == -1)
		{
			perror("ERROR ATTACHING TO MESSAGE QUEUE");
			exit(1);
		}
}


/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

	message rcvMsg;
	if( msgrcv(msqid, &rcvMsg, sizeof(rcvMsg), SENDER_DATA_TYPE, 0) < 0)
	{
		perror("ERROR");
		exit(1);
	}
	
	msgSize = rcvMsg.size;
	cout <<"msgSize = "<<msgSize<<endl;


	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */

	while(msgSize != 0)
	{
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			rcvMsg.mtype = RECV_DONE_TYPE;
			 if( msgsnd(msqid, &rcvMsg,sizeof(rcvMsg) ,0) < 0 );
			{
				perror("ERROR");
				exit(1);
			}
			 iif(msgrcv(msqid, &rcvMsg, sizeof(rcvMsg), SENDER_DATA_TYPE, 0) < 0 )
			 {
			 perror("ERROR");
			 exit(1);
				 
			 }
			 msgSize = rcvMsg.size;
			 cout <<"msgSize = "<<msgSize<<endl;
		}
		/* We are done */
		
		if( msgSize == 0)
		{
			/* Close the file */
			fclose(fp);
			cout <<"FINISHED SENDING FILE"<<endl;
		}
	}
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{		printf("Ctrl C has been pressed...");
		shmdt(sharedMemPtr);
 
 		if(shmdt(sharedMemPtr == 0)
		   {
			   perror("ERROR");
			   exit(1);
		   }
		   
		shmctl(shmid,IPC_RMID,NULL);
		   
		   if(shmctl(shmid,IPC_RMID,NULL) == 0)
		   {
			   perror("ERROR");
			   exit(1);
		   }
			msgctl(msqid, IPC_RMID, NULL);
		      
		      if(msgctl(msqid, IPC_RMID, NULL) == 0)
		   {
			   perror("ERROR");
			   exit(1);
		   }
		   else
			cout <<"Cleaned UP" <<endl;
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	
	cleanUp(shmid, msqid, sharedMemPtr);
	
}

int main(int argc, char** argv)
{

	cout <<"Press control C to cleanup and exit"<<endl;
	signal(SIGINT,ctrlCSignal);
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();
	cleanUp(shmid, msqid, sharedMemPtr);


	return 0;
}
