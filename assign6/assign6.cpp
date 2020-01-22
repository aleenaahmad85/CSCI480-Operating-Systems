/********************************************************************
CSCI 480 - Assignment 6 - Semester (Fall) 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  11/13/19

Purpose:  In this assignment, we are simulating memory management.

*********************************************************************/
#include <fstream>
#include <stddef.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <iostream>
#include "assign6.h"

using namespace std;

//OS has the first 3 MB
int startAddress = 3 * MB;

int main(int argc, char *argv[])
{
	char arguement;
	if(argc > 1)
	{// Check for the number of arguments
		arguement = argv[1][0];// Get argument
		if(!(arguement == 'B' || arguement == 'F' || arguement == 'b' || arguement == 'f'))
		{// Check for correct argument
			cerr << endl << "use the argument 'B' or 'F' or 'b' or 'f'"  << endl;
			exit(-1);
		}
	}
	else
	{
		cerr << endl << "No arguments given" << endl;
		exit(-1);
	}
	int sizes[HOWOFTEN] ={MB, 2 * MB, 2 * MB, 4 * MB, 4 * MB};//makes the memeory spaces
	for(int i = 0; i < HOWOFTEN; i++)
	{
		Avil.push_back(*(new Block(sizes[i], startAddress)));//actually makes the memory with those spaces
		startAddress = startAddress + sizes[i];
	}
	if(arguement == 'B' || arguement == 'b')
	{
		// Start best fit
		cerr << "Simulation of Memory Management using the Best-Fit algorithm" << endl << endl << "Beginning of the run"  << endl << endl;
		bRun();
	}
	else if(arguement == 'F' || arguement == 'f')
	{
		// Start First Fit
		cerr << "Starting Simulation with First-Fit method" << endl << endl << "Beginning of the run" << endl << endl;
		fRun();
	}
	return 0;
}

/***************************************************************
Function:  deallocate

Use:	   deallocate a block of code that was previously allocated

Arguments: string that is the line from the text file that is
	   controlling the program

Returns:   nothing
***************************************************************/
void deallocate(string line)
{
	char command;
	int id;
	string name;
	istringstream str(line);//to grab the whole line
	str >> command >> id >> name;//grab the stuff from the lines and put them in the correct variable

	cerr << "Transaction: request to deallocate block ID " << name << " for process " << id << endl;
	list <Block>::iterator it1;
	for(it1 = InUse.begin(); it1 != InUse.end(); it1++)
	{
		// Loop through InUse blocks
		if(it1->pid == id && it1->name == name)
		{
			// Look for block to deallocate
			insert(*(new Block(it1->size, it1->startAddress)));// Put block into Avil
			InUse.erase(it1);// Remove block from inUse
			cerr << "Success in deallocating a block" << endl;
			return;
		}
	}
}

/***************************************************************
Function:  terminate

Use:	   to kill a process and free all its memory

Arguments: a string that is the next line from the text file that
	   controls the program

Returns:   nothing
***************************************************************/
void terminate(string line)
{
	char command;
	int id;
	istringstream str(line);//grab the whole line
	str >> command >> id;//chop up the line
	list<Block>::iterator it1;
	cerr << "Transaction:  request to terminate process " << id << endl;
	bool isTerminated = false;
	for(it1 = InUse.begin(); it1 != InUse.end(); it1 ++)
	{
		if(it1->pid == id)
		{
			insert(*(new Block(it1->size, it1->startAddress)));// Put block in Avil
			InUse.erase(it1);// Remove from InUse
			it1--;
			isTerminated = true;
		}
	}
	if(!isTerminated)// Check if any blocks were removed
		cerr<<"Unable to comply as the indicated process could not be found." << endl << endl;
	else
		cerr<<"Success in terminating a process" << endl << endl;
}

/***************************************************************
Function:  insert

Use:	   changes the block of memory from InUse to Avil

Arguments: a block of memory to be inserted into

Returns:   nothing
***************************************************************/
void insert(Block mem)
{
	list<Block>::iterator it1;
	list<Block>::iterator it2;

	bool isInAvailable = false;

	for(it1 = Avil.begin(); it1 != Avil.end(); it1++)
	{
		// Look for the block in available
		if(it1->startAddress > mem.startAddress)
		{
			Avil.insert(it1,mem);
			isInAvailable = true;
			break;
		}
	}
	if(!isInAvailable)
	{
		// if not in available, put it there
		Avil.push_front(mem);
	}
	for(it1=Avil.begin(); it1 != --Avil.end(); it1++)
	{
		// Look for blocks to merge
		it2 = it1;
		it2++;
		if((it1->size + it1->startAddress) == (it2->startAddress))
		{
			// if two available blocks are next to eachother, MERGE
			if((it1->size + it2->size) <= 4*MB)
			{
				cerr << "Merging two blocks at " << it1->startAddress << " and " << it2->startAddress << endl;
				it1->size = it1->size + it2->size;
				Avil.erase(it2);
				it1--;
			}
		}
	}
}

/***************************************************************
Function:  bRun

Use:	   loops through the text file if there is a B in the
	   command line arguments and calls bLoad to fill the memory
	   block in Best-Fit order

Arguments: none

Returns:   nothing
***************************************************************/
void bRun()
{
	ifstream infile;
	infile.open("data6.txt");// Get File

	if(infile.fail())
	{
		cerr << "Did not open correctly" << endl;
		exit(-1);
	}

	string line;
	char command;
	print();// Print systems state
	while(infile)
	{
		getline(infile,line);
		command = line[0];
		if(command == 'L')// Load process
			bLoad(line);

		else if(command == 'T')// Terminate process
			terminate(line);

		else if(line[0] == 'A')// Allocate memory
			bLoad(line);

		else if(command == 'D')// Deallocate memory
			deallocate(line);

		else if(command == '?')// End
			break;

		print();// Print systems state
	}
	infile.close();
	cerr << "Simulation Ended" << endl << endl;
	print();// Print systems state
}

/***************************************************************
Function:  bLoad

Use:	   used to fill the memory block in Best-Fit order

Arguments: string from the text file that controls the program
	   telling it what it put in and take out

Returns:   nothing
***************************************************************/
void bLoad(string line)
{
	char command;
	int id;
	int size;
	string name;
	bool isavailble = false;
	istringstream str(line);// Use line as input
	str >> command >> id >> size >>name;// Break apart input line

	list<Block>::iterator it1;
	list<Block>::iterator it2;

	int memoryAvl = 15 * MB;
	if(command == 'L')
		cerr << "Transaction:  request to load process " << id << ", block ID " << name << " using " << size << " bytes" << endl;
	else
		cerr << "Transaction:  request to allocate " << size << " bytes for process " << id << ", block ID: " << name << endl;
	for(it1=Avil.begin(); it1 != Avil.end(); ++it1)
	{
		// Find the best fitting memory block
		if((it1->size - size) < memoryAvl && (it1->size - size) > 0)
		{
			memoryAvl = (it1->size - size);
			it2 = it1;
			isavailble = true;
		}
	}
	if(isavailble)
	{
		// if there is one, use it
		cerr << "Found a block of size " << it2->size << endl;
		it2->size = it2->size - size;
		Block temp(size, it2->startAddress);
		it2->startAddress = it2->startAddress + size;
		temp.pid = id;
		temp.name = name;
		InUse.push_back(temp);
		cerr << "Success in allocating a block" << endl << endl;
	}
	else
	{
		cerr << "Unable to comply as no block of adequate size is available" << endl << endl;
	}

}

/***************************************************************
Function:  fRun

Use:	   opens the text file that controls the program and calls
	   the correct function to fill it in First-Fit order

Arguments: none

Returns:   nothing
***************************************************************/
void fRun()
{
	ifstream infile;
	infile.open("data6.txt");// Get File

	if(infile.fail())
        {
                cerr << "Did not open correctly" << endl;
                exit(-1);
        }

	string line;
	char command;
	print();// Print systems state
	while(infile)
	{
		getline(infile,line);
		command = line[0];
		if(command == 'L')// Load process
			fLoad(line);

		else if(command == 'T')// Terminate process
			terminate(line);

		else if(line[0] == 'A')// Allocate memory
			fLoad(line);

		else if(command == 'D')// Deallocate memory
			deallocate(line);

		else if(command == '?')// End
			break;

		print();// Print systems state
	}
	infile.close();// close file
	cerr << "Simulation Ended" << endl;
	print();// Print systems state
}

/***************************************************************
Function:  fLoad

Use:	   used to fill the memory block in First-Fit order

Arguments: string from the text file that controls the program
           telling it what it put in and take out

Returns:   none
***************************************************************/
void fLoad(string line)
{
	char command;
	int id;
	int size;
	string name;
	bool isavailble = false;
	istringstream str(line);// Use line as input
	str >> command >> id >> size >> name;// Break apart input line

	list<Block>::iterator it1;

	if(command == 'L')// Check what we are doing
		cerr << "Transaction:  request to load process " << id << ", block ID " << name << " using " << size << " bytes" << endl;
	else
		cerr << "Transaction:  request to allocate " << size << " bytes for process " << id << ", block ID: " << name << endl;
	for(it1 = Avil.begin(); it1 != Avil.end(); ++it1)
	{
		// Look for the first memory block we can use
		if((it1->size - size) >= 0)
		{
			isavailble = true;
			cerr << "Found a block of size " << it1->size << endl;
			break;
		}
	}
	if(isavailble)
	{
		// If there is a block we can use, start using it
		it1->size = it1->size - size;
		Block temp(size, it1->startAddress);
		it1->startAddress = it1->startAddress + size;
		temp.pid = id;
		temp.name = name;
		InUse.push_front(temp);

		cerr << "Success in allocating a block" << endl << endl;
	}
	else
	{
		cerr << "Unable to comply as no block of adequate size is available" << endl << endl;
	}
}

/***************************************************************
Function:  print

Use:	   to print out the contents of the memory block

Arguments: none

Returns:   none
***************************************************************/
void print()
{
	list<Block>::iterator it1;
	cerr << "List of available blocks" << endl;
	int total = 0;
	if(Avil.size() > 0)
	{
		for(it1=Avil.begin(); it1 != Avil.end(); ++it1)
		{
			// Loop and print availble memory
			cerr << "Start Address =" << setw(9) << it1->startAddress;
			cerr << " Size =" << setw(9) << it1->size << endl;
			total = total + it1->size;
		}
	}
	else
	{
		cerr << "(none)"  << endl;
	}
	cerr << "Total size of the list is: " << total  << endl << endl;
	cerr << "List of blocks in use" << endl;
	total = 0;
	if(InUse.size() > 0)
	{
		for(it1=InUse.begin(); it1 != InUse.end(); ++it1)
		{
			// Loop and print inUse memory
			cerr << "Start Address =" << setw(9) << it1->startAddress;
			cerr << " Size =" << setw(8) << it1->size;
			cerr << " Process ID =" << setw(5) << it1->pid;
			cerr << " Block ID =" << setw(6) << it1->name << endl;
			total = total + it1->size;
		}
	}
	else
	{
		cerr << "(none)" << endl;
	}
	cerr << "Total size of the list is: " << total << endl << endl;
}
