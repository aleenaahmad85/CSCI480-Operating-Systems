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

//some constants used in this program are stored here.
#ifndef ASSIGN4_H
#define ASSIGN4_H

#include "process.h"

const int MAX_TIME = 500;//length of the whole run
const int IN_PLAY = 6;//max number of processe that can be in play at once
const int QUEUE_SIZE = 20;//guaranteed larger than max number of items any queue will ever hold
const int ARRAY_SIZE = 10;//size of History array defined in a process
const int HOW_OFTEN = 25;//how often to reprint the state of the system

#endif
