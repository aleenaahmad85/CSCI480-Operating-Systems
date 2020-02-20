/********************************************************************
CSCI 480 - Assignment X - Semester (Fall) 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  10/14/19

Purpose:   Makes the process


*********************************************************************/
#include <cstring>
#include <string>
#include "Process.h"

//Static memeber PIDNum acting as a PID Generator
int Process::PIDNum = 101;


/***************************************************************
Function:  Process

Use:	   To make a new process with default values

Arguments: None

Returns:   None
***************************************************************/
Process::Process()
{
}


/***************************************************************
Function:  Process

Use:	   Generates a process from 2 lines of input

Arguments: Pointer to a char that holds ProcessName, Priority, & ArrivalTime
	   Pointer to a char that holds ProcessType & ProcessTimer

Returns:   None
***************************************************************/
Process::Process(char * l1, char* l2)
{
	//start tokenizing first line in the form "Processname Priority ArrivalTime"
	char * holder = strtok(l1, " ");
	ProcessName = holder;

	holder = strtok(nullptr, " ");
	Priority = atoi(holder);

	holder = strtok(nullptr, " ");
	ArrivalTime  = atoi(holder);

	//fill "History" array with values
	holder = strtok(l2, " ");

	for (int i = 0; i < ARRAY_SIZE && holder != nullptr; i++)
	{
		History[i][0] = holder[0];

		holder = strtok(nullptr, " ");
		History[i][1] = atoi(holder);

		holder = strtok(nullptr, " ");
	}
	//set all defaults to zero
	Sub = 0;
	CPUTimer = 0;
	IOTimer = 0;
	CPUTotal = 0;
	ITotal = 0;
	OTotal = 0;
	CPUCount = 0;
	ICount = 0;
	OCount = 0;

	//obtain pid & set PIDNum for next process obejct
	ProcessID = PIDNum;
	PIDNum++;

	LastTimeSeen = 0;
	TimeSpentWaiting = 0;
}

/***************************************************************
Function:  SingleClockTick

Use:       Increments and decrements CPUTotal CPUTimer based on what is in History

Arguments: None

Returns:   None
***************************************************************/
void Process::SingleClockTick()
{
	switch (History[Sub][0])
	{
		case 'C':
			CPUTimer--;
			CPUTotal++;
			break;
		case 'I':
			IOTimer--;
			ITotal++;
			break;
		case 'O':
			IOTimer--;
			OTotal++;
			break;
	}
}
