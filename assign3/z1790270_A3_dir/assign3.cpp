/********************************************************************
CSCI 480 - Assignment 3 - Semester (Fall) Year

Progammer: Andrew Scheel
Section:   2
TA:	   Jingwan Li
Date Due:  9/28/19

Purpose:   In this assignment, you will implement a microshell in C/C++.

*********************************************************************/

#include <iomanip>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

int main(void)
{
	//variables
	char buf[1024];//what comes in from the user
	char *command[1024];//the command that comes in from the buffer
	char *command1[35] = { NULL };//the left side of the line that is inputted
	char *command2[35] = { NULL };//the right side of the line that is inputted
	char *temp;

	int pipe1[2];

	pid_t pid1;//fork 1
	pid_t pid2;//fork2
	int status;



	printf("480 Shell> ");
	while (fgets(buf,1024,stdin) != NULL)
	{
		//exit condition = q or quit or exit
		if(buf[0] == 'q' || (buf[0] == 'q' && buf[1] == 'u' && buf[2] == 'i' && buf[3] == 't') || (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't'))
		{
			exit(0);
		}

		buf[strlen(buf)-1] = 0;  //remove the last character. Important!

		command[0] = strtok(buf, "||");//grab everything to the left of the ||
		command[1] = strtok(NULL, "||");//grabs everything that is left....well technically right but still this is a comment don't judge me

		command1[0] = strtok(command[0], " ");//grabs everything until the first space

		int i;
		for(i = 1; (temp = strtok(NULL, " ")) != NULL; i++)
		{
			command1[i] = temp;//puts the arguments in
		}
		command1[i++] = NULL;//sets the null terminator

		if(command[1] != NULL)//if there is something in command 2
		{
			i = 0;

			command2[0] = strtok(command[1], " ");

			for(i = 1; (temp = strtok(NULL, " ")) != NULL; i++)
                	{
                        	command2[i] = temp;//puts the arguments in
                	}
			command2[i++] = NULL;
		}
		if(command2 != NULL)//if there is a piped command
		{
			if(pipe(pipe1) == -1)//checks if the pipe failed or not
			{
				perror("pipe failed");
				exit(-5);
			}
		}

		if ((pid1 = fork()) < 0)//checks if the fork failed
		{
			printf("fork 1 error");
			exit(-5);
		}
		else if (pid1 == 0)
		{ /* child */

			if(command2[0] != NULL)
			{
				close(1);
				dup(pipe1[1]);
				close(pipe1[1]);
				close(pipe1[0]);
				wait(0);
			}

			//executing the first command and its arugments
			execvp(command1[0], command1);
		}
		else
		{//parent
			if(command2[0] != NULL)
			{
				if((pid2 = fork()) < 0)//make and check the fork
				{
					printf("fork 2 error");
					exit(-5);
				}
				else
				{
					//close dup the correct ends of the pipe so the input is coming from the previous command
					close(0);
					dup(pipe1[0]);
					close(pipe1[0]);
					close(pipe1[1]);
					wait(0);

					//executes the 2nd command with input from the 1st
					execvp(command2[0], command2);
				}
			}
			if(command2[0] != NULL)
			{
				//close the rest of the pipes
				close(pipe1[1]);
				close(pipe1[0]);
			}
		}
		if(command2[0] != NULL)
		{
			//wait for fork 1 to finish
			waitpid(pid1, &status, 0);
		}

		//wait for fork 2 to finish
		waitpid(pid2, &status, 0);

		//print out the prompt again
		printf("480 Shell> ");

		//to zero out the commands just in case the next commands are shorter than theese ones
		for(int counter = 0; command1[counter] != '\0'; counter++)
			command1[counter] = NULL;
		for(int counter = 0; command2[counter] != '\0'; counter++)
			command2[counter] = NULL;
	}

	return 0;
}

