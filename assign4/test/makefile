#/********************************************************************
#CSCI 480 - Assignment X - Semester (Fall) 2019

#Progammer: Andrew Scheel
#Section:   2
#TA:        Jingwan Li
#Date Due:  10/14/19
#Purpose:   to compile all the files in the right way to make it work
#*********************************************************************/
# Compiler variables
CCFLAGS = -Wall -std=c++11

# Rule to link object code files to create executable file
Assign4: Assignment4.o Process.o
	g++ $(CCFLAGS) -o Assign4 Assignment4.o Process.o

# Rules to compile source code file to object code
Assignment4.o: Assignment4.cpp Assignment4.h Process.h
	g++ $(CCFLAGS) -c Assignment4.cpp

Process.o: Process.cpp Process.h
	g++ $(CCFLAGS) -c Process.cpp

#Header file dependency
Process.h: Assignment4.h

# Pseudo-target to remove object code and executable files
clean:
	-rm *.o Assign4
