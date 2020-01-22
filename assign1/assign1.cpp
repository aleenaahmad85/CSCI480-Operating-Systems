/********************************************************************
CSCI 480 - Assignment 1 - Semester (Fall) Year

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  9/6/19

Purpose:   This assignment involves using LINUX system functions such
	   as fork(), getpid(), getppid(), wait() and system().
*********************************************************************/

#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
	//1 get the og PIDs
	cerr << "This is from the original process. The PID is " << getpid() << " and my parent's PID is " << getppid() << endl;

	//2 call fork and say you did
	cerr << "Now we have the first fork" << endl;
	int pid = fork();

	//3 if the fork failed get out
	if(pid == -1)
	{
		cerr << "The first fork failed." << endl;
		exit(-1);
	}
	//4
	else if(pid == 0)//child process
	{
		//a go into the child and output the pids
		cerr << "We are now in the child process. The PID for the child is " << getpid() << " and the parent's PID is " << getppid() << endl;
		//b say you're going to fork and then fork
		cerr << "Here we are forking for a second time" << endl;
		int cpid = fork();
		//c check the fork
		if(cpid == -1)//failed
		{
			cerr << "Second fork failed." << endl;
			exit(-1);
		}
		//d
		else if (cpid == 0)//child (grandchild)
		{
			//i output the grandchild pid and then it's parent, the child
			cerr << "We are now in the grandchild process. The PID for the grandchild is " << getpid() << " and the parent(child)'s PID is " << getppid() << endl;
			//ii say about to exit
			cerr << "About to exit the grandchild" << endl;
			//iii leave the child
			exit(0);
		}
		else//parent (child)
		{
			//i output the child and parent pid
			cerr << "We are now in the child process. The PID for the child is " << getpid() << " and the parent's PID is " << getppid() << endl;
			//ii wait for the grandchild to finish
			wait(0);
			//iii say that about to leave
			cerr << "About to exit the child" << endl;
			//iv exit
			exit(0);
		}
	}
	else//parent process
	{
		//a output the parent pid and then the init pid
		cerr << "We are now in the parent process. The PID for the parent is " << getpid() << " and its parent's PID is " << getppid() << endl;
		//b sleep and let the children go first
		sleep(2);
		//c say about to call a system command
		cerr << "About to call ps." << endl;
		//d call the system command
		system("ps");
		//e wait for the children to close
		wait(0);
		//f say parent will end
		cerr << "The parent is about to terminate." << endl;
		//g end the parent
		exit(0);
	}
	//5 return 0
	return 0;
}
