#include <sys/shm.h>
#include <sys/msg.h>
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
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* TODO: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	    2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V objest (i.e. message queues, shared memory, and sempahores) 
		    is unique system-wide among all SYstem V objects. Two objects, on the other hand,
		    may have the same key.
	 */
	key_t key;
	key = ftok("keyfile.txt", 'a');  //generate key
	if(key == -1)
	{
		perror("ERROR GETTING KEY"); //Error checking
		exit(1);
	}
	
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666|IPC_CREAT);  //System call "shmget" that asks for shared memory segment
	if(shmid == -1)
	{
		perror("ERROR RETURNING IDENTIFIER");
		exit(1);
	}
		 
		       
		
	sharedMemPtr = shmat(shmid, (void*)0 ,0); // System call shmat() accepts a shared memory ID: "shmid"
	
	msqid = msgget(key, IPC_CREAT|0666); //Attaching to message queue
	if(msqid == -1)
	{
		perror("ERROR ATTACHING TO MESSAGE QUEUE");
	}
	
	/* TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	/* TODO: Attach to the shared memory */
	/* TODO: Attach to the message queue */
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
	
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	 shmdt(sharedMemPtr); //detaches the shared memory segment located at the address specified by sharedMemPtr
	if(shmdt(SharedMemPtr == -1)
	   {
		   perror("ERROR DETACHING MEMORY SEGMENT");
		   exit(1);
	   }
	
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");
	

	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	message rcvMsg;
	
	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory.
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
			
		 sndMsg.mtype = SENDER_DATA_TYPE;
		cout <<"Message size: "= "<<sndMsg.size<<" bytes."<<endl;

		 int k;
		 k = msgsnd(msqid, &sndMsg,sizeof(sndMsg),0);
		
		 if( k < 0 )
		 {
			 cout <<"message send failed"<<endl; //Error checking
		 }

		 if(msgrcv(msqid, &rcvMsg, sizeof(rcvMsg), RECV_DONE_TYPE, 0) < 0 && (sndMsg.size!=0)) // Error checking (If Sender didn't send and Reciever didn't recieve)
			 
		 {
			 cout <<"ERROR RECIEVING MESSAGE"<<endl;
		 }
		
		 else if(rcvMsg.mtype == RECV_DONE_TYPE) // Error Checking (If the recieved message is equal to "done" message from "msg.h")
			 
		 {
			 	cout <<"RECIEVED MESSAGE..."<<endl;
		 }
	}


	/* Close the file */
	fclose(fp);
	cout <<"Closing File"<<endl;
}



int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
	/* Send the file */
	send(argv[1]);
	
	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}
