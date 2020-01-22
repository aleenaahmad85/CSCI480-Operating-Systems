/********************************************************************
CSCI 480 - Assignment 2 - Semester (Fall) Year

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  9/17/19

Purpose:   This assignment involves using the LINUX system function
	   pipe() as well as fork().

*********************************************************************/


#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

//function prototypes
int pWork(int, int);
int cWork(int, int);
int gWork(int, int);

int main()
{
	//step 1
	//the pipes
	int pipeA[2], pipeB[2], pipeC[2];
/*		      parent
			A
		       / \	Read = 0 Write = 1 Error = 2
		      /	  \
	 Grandchild  C-----B Child
*/

	//step 2 & 3
	//making the pipe and checking if it worked correctly
	if(pipe(pipeA) < 0)
	{
		exit(-5);
	}
	if(pipe(pipeB) < 0)
	{
		exit(-5);
	}
	if(pipe(pipeC) < 0)
	{
		exit(-5);
	}

	//step 4 & 5
	int pfork, cfork;
	//making the fork
	pfork = fork();
	//checking if the fork worked
	if(pfork == -1)
	{
		cerr << "First fork failed";
		exit(-5);
	}
	else if(pfork == 0)
	{
		//we are now in the child process
		//make the fork and then check the fork
		cfork = fork();
		if(cfork == -1)
        	{
                	cerr << "Second fork failed";
                	exit(-5);
        	}
		else if(cfork == 0)
		{
			//we are in the grandchild now
			//the grandchild reads from B, writes to C
				//close write B, read C, both A

			//closing pipes that I don't need
			close(pipeB[1]);
			close(pipeC[0]);
			close(pipeA[0]);
			close(pipeA[1]);

			cerr << "The grandchild is ready to proceed" << endl;

			//call the grandchild funtion
			gWork(pipeB[0], pipeC[1]);

			//close the remaining pipes
			close(pipeB[0]);
			close(pipeC[1]);
			//exit when done
			exit(0);
		}
		else
		{
			//back in the "parent", child, process
			//the child reads from A and writes to B
				//close write A, read B, and both C

			//close pipes that I don't need
			close(pipeA[1]);
                        close(pipeB[0]);
                        close(pipeC[0]);
                        close(pipeC[1]);

			cerr << "The child is ready to proceed\n" << endl;

			//call the child function
			cWork(pipeA[0], pipeB[1]);

			//close the remaining pipes
                        close(pipeA[0]);
                        close(pipeB[1]);
			//wait for the grandchild to finish and then exit
			wait(0);
			exit(0);
		}
	}
	else
	{
		//back in the parent process
		//the parent reads from C and writes to A
			//close the write C and read of A and both of B

		//close pipes that I don't need
		close(pipeC[1]);
                close(pipeA[0]);
                close(pipeB[0]);
                close(pipeB[1]);

		cerr << "The parent is ready to proceed" << endl;

		//call the parent function
		pWork(pipeC[0], pipeA[1]);

		//close the remaining pipes
                close(pipeC[0]);
                close(pipeA[1]);
		//wait for the child to finish and then exit
		wait(0);
		exit(0);
	}

	//step 6
	return 0;//just in case
}

//functions

/***************************************************************
Function:pWork

Use:does the math for the parent process and writes to the correct
    end of the pipe after reading form the correct end

Arguments: reading and writing, the read and write ends

Returns:an integer so it breaks out of the loop
***************************************************************/

int pWork(int reading, int writing)
{
	//variables
	char Buffer[15];
	char Value[15] = {'1'};
	char temp;//to make sure the value isn't \0

	long int M = 1;
	int i = 0;

	//PWork only stuff
		//the parent reads from C and writes to A


	//write the first value to start everything off
	write(writing, Value, strlen(Value)+1);

	//output the first value
	cerr << "Parent:"<< setw(20) <<"Value = " << Value << endl;

	//loop
	while(M < 99999999999)
	{
		while(read(reading, &temp, 1) > 0 && temp != '\0')//reading and checking for \0 at the end of the C string
		{
			Buffer[i] = temp;
			i++;
		}

		//making sure to set the last character to a \0
		Buffer[i] = '\0';
		i = 0;

		//going from string to long int
		M = atol(Buffer);

		M = (3 * M) + 7;//math

		//converting the long int to a string
		sprintf(Value, "%ld", M);

		//output
		cerr << "Parent:" << setw(20) << "Value = " << Value << endl;
		//writing for the next one
		write(writing, Value, strlen(Value)+1);
	}
	return 1;
}

/***************************************************************
Function:cWork

Use:to do the math for the child process and read and write to the
    correct ends of the pipe

Arguments:reading and writing, the read and write ends of the pipe

Returns: an int to break of out the loop
***************************************************************/

int cWork(int reading, int writing)
{
	//reads B writes C
	//variables
	char Buffer[15];
	char Value[15] = {'1'};
	char temp;//to make sure the value isn't \0

	long int M = 1;
	int i = 0;

	//loop
	while(M < 99999999999)
	{
		while(read(reading, &temp, 1) > 0 && temp != '\0')//reading and checking for \0 at the end of the C string
		{
			Buffer[i] = temp;
			i++;
		}
		//making sure the last charcter is a \0.....like above
		Buffer[i] = '\0';
		i = 0;

		//going from ABC to long 123
		M = atol(Buffer);

		M = (2 * M) + 5;//math

		//converting the long int into a string and storing it in Value
		sprintf(Value, "%ld", M);

		//output
		cerr << "Child:" << setw(21) << "Value = " << Value << endl;

		//passing the value onto the next one
		write(writing, Value, strlen(Value)+1);
	}
	return 1;
}

/***************************************************************
Function:gWork

Use:to do the math for the grandchild and read and write to the
    correct ends of the pipe

Arguments:reading and writing, reading and writing to the correct
	  pipe ends

Returns: an int to break out of the loop
***************************************************************/

int gWork(int reading, int writing)
{
	//reads C writes A
	//variables
	char Buffer[15];
	char Value[15] = {'1'};
	char temp;//to make sure the value isn't \0

	long int M = 1;
	int i = 0;

	//loop
	while(M < 99999999999)
	{
		while(read(reading, &temp, 1) > 0 && temp != '\0')//reading and checking for \0 at the end of the C string
		{
			Buffer[i] = temp;
			i++;
		}
		//same as above we are putting \0 as the last character... and I am wishing I made a seprate function for this cause it repeats
		Buffer[i] = '\0';
		i = 0;

		//easy as going from ABC to long 123
		M = atol(Buffer);

		M = (5 * M) + 1;//math

		//converting from long int to string
		sprintf(Value, "%ld", M);

		//output
		cerr << "Grandchild:" << setw(16) << "Value = " << Value << endl;

		//passing on the value
		write(writing, Value, strlen(Value)+1);
	}
	return 1;
}
