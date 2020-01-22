/********************************************************************
CSCI 480 - Assignment 5 - Semester (Fall) 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  10/25/19

Purpose:   In this assignment, we are going to use POSIX threads, semaphores
	   and a mutex to illustrate the Producer-Consumer Problem.

*********************************************************************/

#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <string>

/*
Producer will be doing:
wait(NotFull)
Insert()
post(NotEmpty)



Consumer will be doing:
wait(NotEmpty)
Remove()
post(NotFull)

*/

using namespace std;

#define P_NUMBER 7//number of producers
#define C_NUMBER 5//number of consumers
#define P_STEPS 5//number of iterations for each producer thread
#define C_STEPS 7//number of iterations for each consumer thread
#define BUFFER_SIZE 35

queue <string> Widgets;
int widgetCount [P_NUMBER] = {0, 0, 0, 0, 0, 0, 0};

//prototypes
void Insert(void*);
void Remove(void*);
void *Produce(void*);
void *Consume(void*);

//makes semaphore and mutex
sem_t NotFull;
sem_t NotEmpty;
pthread_mutex_t lock;
int counter = 0;//number of widgets in the buffer

int main()
{
	cout << "Simulation of Producer and Consumers" << endl << endl;

	//initilize the semaphores and mutex
	sem_init(&NotFull, 0, BUFFER_SIZE);
	sem_init(&NotEmpty, 0, 0);

	pthread_mutex_init(&lock, NULL);

	cout << "The semaphores and mutex have been initialized" << endl << endl;


	pthread_t producerThreads [P_NUMBER];
	pthread_t consumerThreads [C_NUMBER];

	//fills everything up
	for(long i = 0; i < P_NUMBER; i++)
		pthread_create(&producerThreads[i], NULL, Produce, (void *) i);
	for(long j = 0; j < C_NUMBER; j++)
                pthread_create(&consumerThreads[j], NULL, Consume, (void *) j);


	//waits for everything to empty
        for(int i = 0; i < P_NUMBER; i++)
		pthread_join(producerThreads[i], NULL);
	for(int i = 0; i < C_NUMBER; i++)
		pthread_join(consumerThreads[i], NULL);


	pthread_mutex_destroy(&lock);// destroy mutex
	sem_destroy(&NotFull);// destroy semaphore
	sem_destroy(&NotEmpty);// destroy semaphore

	cout << endl << "All the the producer and consumer threads have been closed." << endl << endl;
	cout << "The semaphores and mutex have been deleted." << endl;

	return 0;
}

/***************************************************************
Function:  Produce

Use:	   calls insert to add a widget to the queue

Arguments: thread id

Returns:   none
***************************************************************/
void *Produce(void* id)
{
//need a loop to call Insert P_STEPS times
	int i = 0;
	while(i < P_STEPS)
	{
		//each time insert is called, sleep for 1 second
		sem_wait(&NotFull);
		Insert(id);
		sem_post(&NotEmpty);
		sleep(1);
		i++;
	}
	pthread_exit(NULL);
}

/***************************************************************
Function:  Insert

Use:	   inserts a widget into the queue and displays the queue

Arguments: thread id

Returns:   none
***************************************************************/
void Insert(void* id)
{
/*
Lock mutex
add widget to buffer
increment the counter
unlock mutext
*/
	//lock the mutex
	pthread_mutex_lock(&lock);
	//increment counter
	counter++;
	//add widget to buffer

	long tempID = (long) id;
	string temp = "P";
	temp += to_string(tempID);
	temp += "W";
	temp += to_string(widgetCount[tempID]);

	Widgets.push(temp);
	widgetCount[tempID] += 1;


	//should print out a message whenever they are called
	//	stating id number of calling thread, producer or consumer, what it just did, value of counter
	cout << "Producer " << (long)id << " added one item. Total is now " << counter << endl;
	cout << "Buffer now contains: ";

	//to display the contents of the queue
	queue <string> tempQ = Widgets;
	while(!tempQ.empty())
	{
		cout << "  " << tempQ.front();
		tempQ.pop();
	}

	cout << endl << endl;

	//unlock the mutex
	pthread_mutex_unlock(&lock);
}

/***************************************************************
Function:  Consume

Use:	   calls Remove to remove one from the queue

Arguments: thread id

Returns:   none
***************************************************************/
void *Consume(void* id)
{
//need a loop to call Remove C_STEPS times
	int j = 0;
	while(j < C_STEPS)
	{
		//each time insert is called, sleep for 1 second
		sem_wait(&NotEmpty);
		Remove(id);
		sem_post(&NotFull);
		sleep(1);
		j++;
	}
	pthread_exit(NULL);
}

/***************************************************************
Function:  Remove

Use:	   Removes a widget from the queue and outputs the contents of the queue

Arguments: the id of the thread

Returns:   none
***************************************************************/
void Remove(void* id)
{
/*
lock mutex
get widget from buffer
decrement the counter
unlock mutext
*/
	//lock mutex
	pthread_mutex_lock(&lock);
	//decrement counter
	counter--;
	//get wiget from buffer
	Widgets.pop();

	//should print out a message whenever they are called
	//	stating id number of calling thread, producer or consumer, what it just did, value of counter
	cout << "Consumer " << (long)id << " removed one item. Total is now " << counter << endl;
	cout << "Buffer now contains: ";


	//to display the contents of the queue
        queue <string> tempQ = Widgets;
        while(!tempQ.empty())
        {
                cout << "  " << tempQ.front();
                tempQ.pop();
        }

        cout << endl << endl;

	//unlock mutex
	pthread_mutex_unlock(&lock);
}
