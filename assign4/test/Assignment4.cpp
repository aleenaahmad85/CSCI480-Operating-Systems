/********************************************************************
CSCI 480 - Assignment 4 - Semester Fall 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  Monday, October 14th 2019

Purpose:   In this assignment, we are simulating priority scheduling of processes
           on a single-processor system.  The idea is that when a process ends its
           CPU burst (or input burst or output burst), it is succeeded by the highest-priority
           process that is waiting.

*********************************************************************/

#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>
#include <string>
#include <queue>

#include "Process.h"
#include "Assignment4.h"

using namespace std;

//Function prototypes

void FillEntry();
bool isFinished();
int howManyInSystem();
void PrintStatus();
void Terminate(Process*);

//Input file name
const char* fileName = "./data4.txt";

//specialized priority queue
typedef priority_queue<Process, vector<Process>, GreaterPriorityOperator> ProcessPriorityQueue;

//regular entry queue
queue<Process> Entry;

//P queue objects
ProcessPriorityQueue Ready;
ProcessPriorityQueue Input;
ProcessPriorityQueue Output;

//Process pointers
Process* Active = nullptr;
Process* IActive = nullptr;
Process* OActive = nullptr;

//Process holders for Processers that are currently doing work
Process PushHolder;
Process APHolder;
Process IPHolder;
Process OPHolder;

//other Important global variables
int Timer = 0;
int NoTermProcess = 0;

int main()
{
	//fill the Entry queue and initalize varables
	FillEntry();
	Timer = 0;
	bool waitProcessExsists = true;

	cout << "Simulation of Priority Scheduling\n\n";

	//Main run loop
	do
	{
		//retrive waiting processes if we can do so
		while (howManyInSystem() < AT_ONCE && waitProcessExsists)
		{
			//Hold the Entering prcoess, and pop it off entry
			PushHolder = Entry.front();
			Entry.pop();

			//print step
			cout << "Process " << Entry.front().ProcessID << " moved from Entry Queue into the Ready Queue at time " << Timer << endl << endl;

			//set default for start time, and start "LastTimeSeen" (wait-time)
			PushHolder.LastTimeSeen = Timer;
			PushHolder.StartTime = Timer;

			//Push process onto ready queue
			Ready.push(PushHolder);

			//Make sure not to enter "stop here", or a process that has yet to arrive
			waitProcessExsists = !(Entry.front().ArrivalTime > Timer || Entry.front().ProcessName == "STOPHERE");
		}

		//Check Active pointer and Ready queue
		if (Active == nullptr && !Ready.empty())
		{
			//get process from Ready to pop() & set Active pointer
			APHolder = Ready.top();
			Active = &APHolder;
			Ready.pop();

			//Calculate (Total)TimeSpentWait
			Active->TimeSpentWaiting += (Timer - Active->LastTimeSeen);

			//Start CPUTimer countdown
			Active->CPUTimer = (short) Active->History[Active->Sub][1];
		}

		//If a process was active, add a clock tick
		if (Active != nullptr)
		{
			//Execute a clock tick based on where we are in History[Sub][0]
			Active->SingleClockTick();

			//is the CPU burst finished?
			if (Active->CPUTimer == 0)
			{
				//increment CPU Burst Counter
				Active->CPUCount++;

				//increment subscript for History
				Active->Sub++;

				//What to do with process next?
				switch(Active->History[Active->Sub][0])
				{
					//input
					case 'I':
						Input.push(*Active);
						break;
					//output
					case 'O':
						Output.push(*Active);
						break;
					//to terminate
					default:
						Terminate(Active);
				}
				//Update LastTimeSeen for TimeSpentWaiting
				Active->LastTimeSeen = Timer;

				//Remove process from Ready & free our active pointer
				Active = nullptr;
			}
		}
		//Idle time!
		else
		{
			cout << endl << "At time "  << Timer << ", Active is 0, so we have idle time for a while" << endl << endl;
		}

		//Get a I process if we can
		if (IActive == nullptr && !Input.empty())
		{
			//get process from Input to pop() & set IActive pointer
			IPHolder = Input.top();
			IActive = &IPHolder;
			Input.pop();

			//Calculate (Total)TimeSpentWait
			IActive->TimeSpentWaiting += (Timer - IActive->LastTimeSeen);

			//Start IOTimer countdown
			IActive->IOTimer = (short) IActive->History[IActive->Sub][1];
		}

		if (IActive != nullptr)
		{
			//Execute a clock tick based on where we are in History[Sub][0]
			IActive->SingleClockTick();

			//Are we done with our I Burst?
			if (IActive->IOTimer == 0)
			{
				//Increment In Burst counter
				IActive->ICount++;

				//increment our history subscript
				IActive->Sub++;

				//update LastTimeSeen (Wait Timer)
				IActive->LastTimeSeen = Timer;

				//Push back onto to Ready
				Ready.push(*IActive);

				//Our Input is now free again
				IActive = nullptr;
			}
		}

		if (OActive == nullptr && !Output.empty())
		{
			//get process from Output to pop() & set OActive pointer
			OPHolder = Output.top();
			OActive = &OPHolder;
			Output.pop();

			//Calculate (Total)TimeSpentWait
			OActive->TimeSpentWaiting += (Timer - OActive->LastTimeSeen);

			//Start IOTimer countdown
			OActive->IOTimer = (short) OActive->History[OActive->Sub][1];
		}
		if (OActive != nullptr)
		{
			//Execute a clock tick based on where we are in History[Sub][0]
			OActive->SingleClockTick();

			//Are we done with our O Burst?
			if (OActive->IOTimer == 0)
			{
				//Increment out Burst counter
				OActive->OCount++;

				//increment our history subscript
				OActive->Sub++;

				//update LastTimeSeen (Wait Timer)
				OActive->LastTimeSeen = Timer;

				//Push back onto to Ready
				Ready.push(*OActive);

				//Our Output is now free again
				OActive = nullptr;
			}
		}
		//output status when Timer is a multiple of HOW_OFTEN
		if ((Timer % HOW_OFTEN) == 0)
			PrintStatus();
		//Increment clock tick
		Timer++;

		//Make sure not to enter "stop here"
		waitProcessExsists = !(Entry.front().ArrivalTime > Timer || Entry.front().ProcessName == "STOPHERE");

	} while (!isFinished());//do whiles are stupid but it works this way so its staying

	//Final output
	cout << endl << endl << "The run has ended" << endl << "The Final Value of the Timer was: " << Timer << endl << "Number of terminated Processes = " << NoTermProcess << endl << "Final ";

	//Final Print
	PrintStatus();

	return 0;
}

/***************************************************************
Function:  howManyInSystem

Use:       Counter how many processes are in the system

Arguments: None

Returns:   The number of processes in the system
***************************************************************/
int howManyInSystem()
{
	int rv = Ready.size() + Input.size() + Output.size();
	if (Active != nullptr)
		rv++;
	if (IActive != nullptr)
		rv++;
	if (OActive != nullptr)
		rv++;
	return rv;
}

/***************************************************************
Function:  isFinished

Use:       checkes to see if the program is done

Arguments: None

Returns:   True if no processes left
	   False if processes left
***************************************************************/
bool isFinished()
{
	if(Timer == MAX_TIME)
		return MAX_TIME;
	else
		return Ready.empty() && Input.empty() && Output.empty() && Active == nullptr && IActive == nullptr && OActive == nullptr;
}

/***************************************************************
Function:  FillEntry

Use:  	   Fills the Entry array with processes

Arguments: None

Returns:   None
***************************************************************/


void FillEntry()
{
	//attempt to open file
	ifstream inFile;
	inFile.open(fileName);

	//checks if there is an error or not
	if (!inFile)
	{
		cerr << "File Failed";
		exit(-1);
	}

	//read two lines at a time
	char input1[256];
	char input2[256];

	//sets the last character in the array to null
	memset(input1, '\0', 256);
	memset(input2, '\0', 256);

	//read loop
	while (inFile && inFile.peek() != -1)
	{
		//carriage return and newline at the beginning of a newline
		//indicates we may be at the end of the file...
		if (inFile.peek() == '\n' || inFile.peek() == '\r')
			inFile.ignore();
		else
		{
			//read two lines
			inFile.getline(input1, 256);
			inFile.getline(input2, 256);

			//Generate a new process from the two read lines and push it onto Entry
			Entry.push(Process(input1, input2));

		        //sets the last character in the array to null
			memset(input1, '\0', 256);
			memset(input2, '\0', 256);
		}
	}
	//close file
	inFile.close();
}

/***************************************************************
Function:  PrintStatus

Use:	   Used for printing all the things

Arguments: None

Returns:  None
***************************************************************/
void PrintStatus()
{
	//holder variables
	queue<Process> tempq;
	ProcessPriorityQueue temppq;

	cout << "Status at time " << Timer << endl;
	cout << "Active is at " << ((Active == nullptr)? 0 : Active->ProcessID) << endl;
	cout << "IActive is at " << ((IActive == nullptr)? 0 : IActive->ProcessID) << endl;
	cout << "OActive is at " << ((OActive == nullptr)? 0 : OActive->ProcessID) << endl;
	cout << "Contents of the Entry Queue:" << endl;

	//Print everyhing in Empty
	while (!Entry.empty())
	{
		if (Entry.front().ProcessName != "STOPHERE")
			cout << Entry.front().ProcessID << " Priority = " << Entry.front().Priority << " arriving at time = " << Entry.front().ArrivalTime << endl;
		tempq.push(Entry.front());
		Entry.pop();
	}

	//'refill' Entry
	Entry = tempq;

	//empty out our temp queue
	while (!tempq.empty())
		tempq.pop();

	cout << "Contents of the Ready Queue:" << endl;

	//Print everything in Ready with ProcessID & Priority
	if (Ready.empty())
		cout << "(Empty)";
	else
	{
		while (!Ready.empty())
		{
			cout << Ready.top().ProcessID << "(" << Ready.top().Priority << ")   ";
			temppq.push(Ready.top());
			Ready.pop();
		}

		//'refill' Ready
		Ready = temppq;
		//empty out our temp priority queue
		while (!temppq.empty())
			temppq.pop();
	}
	cout << endl;

	cout << "Contents of the Input Queue:" << endl;

	//Print everything in Input with ProcessID & Priority
	if (Input.empty())
		cout << "(Empty)";
	else
	{
		while (!Input.empty())
		{
			cout << Input.top().ProcessID << "(" << Input.top().Priority << ")   ";
			temppq.push(Input.top());
			Input.pop();
		}

		//'refill' Input
		Input = temppq;
		//empty out our temp priority queue
		while (!temppq.empty())
			temppq.pop();
	}
	cout << endl;
	cout << "Contents of the Output Queue:" << endl;

	//Print everything in Output with ProcessID & Priority
	if (Output.empty())
		cout << "(Empty)";
	else
	{
		while (!Output.empty())
		{
			cout << Output.top().ProcessID << "(" << Output.top().Priority << ")   ";
			temppq.push(Output.top());
			Output.pop();
		}

		//'refill' Output
		Output = temppq;
		//empty out our temp priority queue
		while (!temppq.empty())
			temppq.pop();
	}

	cout << endl << endl;
}

/***************************************************************
Function:  Terminate

Use:	   Prints out a process's stats and increments Terminate counter

Arguments: a pointer to a process

Returns:   None
***************************************************************/

void Terminate(Process* x)
{
	cout << endl << "A Process has terminated." << endl;
	cout << setw(22) << left << "Process ID:";
	cout << x->ProcessID << endl;
	cout << setw(22) << left << "Name:";
	cout << x->ProcessName << endl;
	cout << setw(22) << left << "Priority:";
	cout << x->Priority << endl;
	cout << setw(22) << left << "Started at time:";
	cout << x->StartTime << " and ended at time " << Timer << endl;
	cout << setw(22) << left << "Total CPU time:";
	cout << x->CPUTotal << " in " << x->CPUCount << " CPU Burst(s)" << endl;
	cout << setw(22) << left << "Total Input time:";
	cout << x->ITotal << " in " << x->ICount << " Input Burst(s)" << endl;
	cout << setw(22) << left << "Total Output time:";
	cout << x->OTotal << " in " << x->OCount << " Output Burst(s)" << endl;
	cout << setw(22) << left << "Time spend waiting:";
	cout << x->TimeSpentWaiting << endl << endl;

	NoTermProcess++;
}
