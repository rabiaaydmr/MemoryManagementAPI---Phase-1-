#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 5
#define MEMORY_SIZE 1000

//Rabia Aydemir 23492

struct node
{
	int id;
	int size;
};


queue<node> myqueue; 
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_t server; 
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size



void release_function()
{
	//This function will be called
	//whenever the memory is no longer needed.
	//It will kill all the threads and deallocate all the data structures.
}

void my_malloc(int thread_id, int size)
{

	//This function will add the struct to the queue
	pthread_mutex_lock(&sharedLock);	
	node k;
	k.id=thread_id;
	k.size=size;
	myqueue.push(k);
	pthread_mutex_unlock(&sharedLock); //unlock

}

void * server_function(void *)
{
	//This function should grant or decline a thread depending on memory size.
	
	int index=0;
	bool check=true;
	int as=0;
	while (check)
	{

		if(!myqueue.empty())
		{
				
			pthread_mutex_lock(&sharedLock);	//lock

			node k=myqueue.front();
			myqueue.pop();
			as ++;
			 if(k.size<MEMORY_SIZE-index)//if the memory size okay :)
			{
				thread_message[k.id]=index;
				index= index +k.size +1;
		
				
			}
			else//not enough memory :)
			{				
				thread_message[k.id]=-1;		
	
			}
			 	sem_post(&(semlist[k.id])); //up

				
				pthread_mutex_unlock(&sharedLock); //unlock  
				
				if(as==5)
				
				check=false;
		}
		
	
	}
}

void * thread_function(void * id) 
{
	//This function will create a random size, and call my_malloc
	//Block
	int* idthread=(int*) id;
	int size= rand() % (300) ;

	my_malloc(*idthread , size);
	
	sem_wait(&(semlist[*idthread]));// down
	pthread_mutex_lock(&sharedLock);

	
	if(thread_message[*idthread]== -1)
	{
		cout<<"Thread "<< *idthread<<": Not enough memory \n";
	}
	else
	{
		for (int i=0; i<= size ; i++)
		{
			char num='1';
			memory[thread_message[*idthread]+i]=num;
		}
	}
	
	pthread_mutex_unlock(&sharedLock); //unlock
	//Then fill the memory with 1's or give an error prompt
}

void init()	 
{
	pthread_mutex_lock(&sharedLock);	
	for(int i = 0; i < NUM_THREADS; i++) 
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
  	{char zero = '0'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server 
	pthread_mutex_unlock(&sharedLock); 
}



void dump_memory() 
{
 // You need to print the whole memory array here.
	for(int i =0; i<MEMORY_SIZE; i++)
	{
		cout<<memory[i]<< " ";
	}
}

int main (int argc, char *argv[])
 {

	

 	//You need to create a thread ID array here
	 int  threadid[NUM_THREADS];
	 for(int i=0; i<NUM_THREADS;i++)
	 {
		 threadid[i]=i;
	 }
 	init();	// call init
	pthread_t idth[NUM_THREADS];
	for(int i=0;i<NUM_THREADS;i++)
	{
		pthread_create (&idth[i],NULL,&thread_function, (void*) &threadid[i]);
	}


 	//You need to create threads with using thread ID array, using pthread_create()

 	//You need to join the threads
	for(int i=0;i<NUM_THREADS;i++)
	{
		pthread_join (idth[i],NULL);
	}
		pthread_join(server,NULL);

 	dump_memory(); // this will print out the memory




 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
 	}
 	printf("\nTerminating...\n");
 }