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

//header file of the process class/struct and the data type
//(call it event) of elements stored in the History array.
#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

struct Event
{
	string processName;
	int priority;//non-negative
	int processID;
	int arrivalTime;
	char history [10][10];
	int sub;//index to history
	long int IOTimer;
	long int CPUTotal;
	long int ITotal;
	long int OTotal;
	long int CPUCount;
	long int ICount;
	long int OCount;
};
#endif
