#Andrew Scheel
CC = g++
CCFLAGS = -Wall -pthread -std=c++11 -g

Assign5 : assign5.o
	$(CC) $(CCFLAGS) -o Assign5 assign5.o

assign5.o : assign5.cpp
	$(CC) $(CCFLAGS) -c assign5.cpp

clean :
	- rm *.o
	- rm Assign5
