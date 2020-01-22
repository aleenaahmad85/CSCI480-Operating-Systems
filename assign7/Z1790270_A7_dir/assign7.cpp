/********************************************************************
CSCI 480 - Assignment 7 - Semester (Fall) 2019

Progammer: Andrew Scheel
Section:   2
TA:        Jingwan Li
Date Due:  12/2/19

Purpose:   This assignment involves maintaining a File Allocation Table.

*********************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stddef.h>
#include <list>
//#include <vector>
#include "Entry.cpp"//vector is included in this file

#define BYTES_PER_BLOCK 512
#define HOW_OFTEN 6
#define MAX_ENTRIES 12
#define FAT_PRINT 240

using namespace std;

//prototypes
void copyFile(string, string, bool);
void deleteFile(string, bool);
void newFile(string, int, bool);
void modifyFile(string, int, bool);
void renameFile(string, string, bool);
void print();
bool searchName(string);

//variables
vector<Entry> entries;
vector<int> dir(4096);//the size of the directory that we are simulating
int blockNum = 1;

int main(int argc, char ** argv)
{
	//zeroing out the directory
	for(int i = 0; (unsigned)i < dir.size(); i++)
		dir[i] = 0;

	cout << "Beginning of the FAT simulation" << endl << endl;

	//put some starting stuff in the table
	newFile(".", 512, false);//current dir
	newFile("..", 0, false);//parent dir

	//open the file
	ifstream infile;
	infile.open("data7.txt");

	//error checking
	if(infile.fail())
	{
		cout << "data7.txt did not open";
		exit(-1);
	}

	//input from the file
	string line;//the whole line from the input file
	string fileName, newFileName;//the original file name and the new file name
	char operation;//which transaction is being done
	int fileSize;//the size of the file

	//for how often stuff will print
	int counter = 0;

	//switch statment to determine what operation needs to be called
	while(infile)
	{
		//check the print
		if(counter % HOW_OFTEN == 0)
		{
			print();
		}

		//get the next line of the file
		getline(infile, line);

		//put the line into a stream
		istringstream ss(line);

		//start to grab stuff from the stream
		ss >> operation;

		//without this it will have go through 3 times with a ?....idk code is weird
		if(operation == '?')
			break;

		switch(operation)
		{
			case 'C':
				ss >> fileName;
				ss >> newFileName;
				cout << "Transaction: Copy a file";
				copyFile(fileName, newFileName, true);
				break;
			case 'D':
				ss >> fileName;
				cout << "Transaction: Delete a file";
				deleteFile(fileName, true);
				break;
			case 'N':
				ss >> fileName;
				ss >> fileSize;
				cout << "Transaction: Add a new file";
				newFile(fileName, fileSize, true);
				break;
			case 'M':
				ss >> fileName;
				ss >> fileSize;
				cout << "Transaction: Modify a file";
				modifyFile(fileName, fileSize, true);
				break;
			case 'R':
				ss >> fileName;
				ss >> newFileName;
				cout << "Transaction: Rename a file";
				renameFile(fileName, newFileName, true);
				break;
			default:
				cout << "? end" << endl;
				break;
		}
		counter++;
	}
	infile.close();
	cout << endl << "End of the FAT Simulation" << endl;

	return 0;
}

/***************************************************************
Function:  copyFile

Use:	   used to copy a file to another name with the same size

Arguments: the file name that will be copied, the new file name,
	   and an argument telling if the main called the function
	   or if it was another function

Returns:   nothing

***************************************************************/
void copyFile(string fileName, string newFileName, bool command)
{
	//search for the name
	int tempSize;
	bool temp = searchName(fileName);
	//if it doesn't exist throw error
	if(temp == false && command == true)
	{
		cerr << endl << fileName << " does not exist" << endl;
		return;
	}

	//serach for new name
	temp = searchName(newFileName);

	//if it does exist throw error
	if(temp == true && command == true)
	{
		cerr << endl << newFileName << " already exists" << endl;
		return;
	}
	//create a new dir entry w/the new name with same size and allocate space for it
	for(int i = 0; (unsigned)i < entries.size(); i++)
	{
		if(entries.at(i).name == fileName)
			tempSize = entries.at(i).size;
	}

	newFile(newFileName, tempSize, false);

	//will only print the following line if the main called the function, not if another function did
	if(command)
		cout << endl << "Successfully copied an existing file, " << fileName << " to a new file, " << newFileName << endl << endl << endl;
}

/***************************************************************
Function:  deleteFile

Use:	   used to deallocate a file once it is deleted

Arguments: the name of the file to be deleted and an argument
	   telling if the main called the function or if it was
	   another function

Returns:   Nothing

***************************************************************/
void deleteFile(string fileName, bool command)
{
	//search for the name
        bool temp = searchName(fileName);
        //if it doesn't exist throw error
        if(temp == false && command == true)
	{
                cerr << endl << fileName << " does not exist" << endl;
		return;//goes back to main
	}
	else
	{
		for(int i = 0; (unsigned)i < entries.size(); i++)
		{
			if(fileName == entries.at(i).name)
			{
				for(int j = 0; (unsigned)j < entries.at(i).blocks.size(); j++)
					dir.at(entries.at(i).blocks.at(j)) = 0;

				entries.erase(entries.begin() + i);
			}
		}

		//will only print the following line if the main called the function, not if another function did
		if(command)
			cout << endl << "Successfully deleted a the file: " << fileName << endl << endl;
	}
}

/***************************************************************
Function:  newFile

Use:	   To make a new file

Arguments: the name of the file, the size of the file, and an
	   argument telling if the main called the function or
	   if it was another function

Returns:   Nothing

***************************************************************/
void newFile(string fileName, int fileSize, bool command)
{
	//search for the name
        bool temp = searchName(fileName);
        //if it doesn't exist throw error
        if(temp == true && command == true)
	{
                cerr << endl << fileName << " already exists" << endl;
		return;//return to main
	}
	//create dir entry with name and size and allocate space
	Entry entry;
	entry.name = fileName;
	entry.size = fileSize;

	//the amount of blocks that an item can take up
	int amountOfBlocks = fileSize / BYTES_PER_BLOCK;
	if(fileSize % BYTES_PER_BLOCK > 0)
		amountOfBlocks++;

	for(int i = 1; i <= amountOfBlocks;)
	{
		for(int j = 0; (unsigned)j < dir.size(); j++)
		{
			if(dir.at(j) == 0 && i == amountOfBlocks)
			{
				dir.at(j) = -1;
				blockNum++;
				entry.blocks.push_back(j);
				i++;
				break;
			}
			else if(dir.at(j) == 0)
			{
				dir.at(j) = blockNum;
				blockNum++;
				entry.blocks.push_back(j);
				i++;
				break;
			}
		}
	}

	//will only print the following line if the main called the function, not if another function did
	if(command)
		cout << endl << "Successfully added a new file, " << fileName << ", of size "<< fileSize << endl << endl;

	entries.push_back(entry);
	return;
}

/***************************************************************
Function:  modifyFile

Use:	   to change the size of a file

Arguments: the name of the file to be modified, the new size of the file
	   and an argument telling if the main called the functionor if it
	   was another function

Returns:   Nothing
***************************************************************/
void modifyFile(string fileName, int newFileSize, bool command)
{
	//search for the name
        bool temp = searchName(fileName);
        //if it doesn't exist throw error
        if(temp == false && command == true)
                cerr << endl << fileName << " does not exist" << endl;

	//new to make a new file with temp file name and new size
	newFile("FILENAME.TEMP", newFileSize, false);

	//delete to get rid of old file
	deleteFile(fileName, false);

	//rename to change temp filename to name of oldfile
	renameFile("FILENAME.TEMP", fileName, false);

	//will only print the following line if the main called the function, not if another function did
	if(command)
		cout << endl << "Successfully modified a file, " << fileName << endl << endl << endl;
}

/***************************************************************
Function:  renameFile

Use:	   To rename a file

Arguments: The original name of the file, the name the file will be turned to,
	   and an argument telling if the main called the functionor if it was
	   another function

Returns:   Nothing

***************************************************************/
void renameFile(string fileName, string newFileName, bool command)
{
	//search for the name
        bool temp = searchName(fileName);
        //if it doesn't exist throw error
        if(temp == true && command == true)
	{
                cerr << endl << fileName << " already exist" << endl;
		return;
	}
	//search for the name
        temp = searchName(newFileName);
        //if it doesn't exist throw error
        if(temp == true && command == true)
	{
                cerr << endl << newFileName << " already exist" << endl;
		return;
	}
	//change name
	for(int i = 0; (unsigned)i < entries.size(); i++)
	{
		if(entries.at(i).name == fileName)
		{
			entries.at(i).name = newFileName;
		}
	}

	//will only print the following line if the main called the function, not if another function did
	if(command)
		cout << endl << "Successfully changed the file name " << fileName << " to the new name of " << newFileName << endl << endl << endl;
}

/***************************************************************
Function:  print

Use:	   To print out the FAT table and the clusters that were used

Arguments: None

Returns:   Nothing

***************************************************************/
void print()
{
	//print the dir and FAT
	int j = 0;
	int totalSize = 0;

	cout << endl << "Directory Listing" << endl;

	//prints what is in the directory and the cluster that the files take up
	for(int i = 0; (unsigned)i < entries.size(); i++)
	{
		cout << "File Name: " << setw(20) << left << entries.at(i).name;
		cout << "     File Size " << entries.at(i).size << endl;

		totalSize += entries.at(i).size;

		cout << "Clusters used:";
		for(int k = 0; (unsigned)k < entries.at(i).blocks.size(); k++)
		{
			if(k % MAX_ENTRIES == 0 && k != 0)
			{
				cerr << "\n                  ";
			}
			cout << setw(6) << right << entries.at(i).blocks.at(k);
		}
		if( entries.at(i).blocks.size() == 0)
		{
			cout << "(none)";
		}
		cout << endl << endl;
	}
	cout << "Files: " << entries.size() << "\t" << "Total Size: " << totalSize << endl << endl;

	//prints what is in the full directory
	for(int i = 0; i < FAT_PRINT; i++)
	{
		if(j % MAX_ENTRIES == 0)
		{
			cout << "#" << setw(3) << setfill('0') << i <<" - " << setw(3) << setfill('0') << i + 11 << ": " << "\t" << setfill(' ');
		}

		cout << right << setw(2) << dir.at(i) << "\t";
		j++;

		if(j % MAX_ENTRIES == 0)
		{
			cout << endl;
			j = 0;
		}
	}
	cout << endl;
}

/***************************************************************
Function:   searchName

Use:	    to check if the name that is passed in is used or not

Arguments:  a name to search for

Returns:    True if the name already exists
	    False if the name doesn't exist

***************************************************************/
bool searchName(string searchName)
{
	//search for searchName
	for(int i = 0; (unsigned)i < entries.size(); i++)
	{
		//if found return true
		if(entries.at(i).name == searchName)
			return true;
	}
	return false;
}
