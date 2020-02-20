/********************************************************************
CSCI 480 - Assignment X - Semester (Fall) 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  10/14/19

Purpose:   To declare all the stuff needed for Process.cpp

*********************************************************************/
//header file of the process class/struct and the data type
//(call it event) of elements stored in the History array.

#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include "Assignment4.h"

using namespace std;

struct Process;
struct GreaterPriorityOperator;

struct Process
{
	Process(char*, char*);
	Process();
	void SingleClockTick();
	static int PIDNum;
	string ProcessName;
	int Priority;//non-negative
	int ProcessID;
	int ArrivalTime;
	int StartTime;
	char History[ARRAY_SIZE][2];
	short int Sub;//index to history
	short int CPUTimer;
	short int IOTimer;
	short int CPUTotal;
	short int ITotal;
	short int OTotal;
	short int CPUCount;
	short int ICount;
	short int OCount;
	int LastTimeSeen;
	int TimeSpentWaiting;
};

struct GreaterPriorityOperator
{
	bool operator()(const Process& lhs, const Process& rhs) const
	{
		if (lhs.Priority == rhs.Priority)
			return (lhs.LastTimeSeen) > (rhs.LastTimeSeen);
		else
			return lhs.Priority < rhs.Priority;
	}
};

#endif
