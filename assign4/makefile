#Andrew Scheel
Assign4 : assign4.o process.o
	g++ -Wall -o Assign4 assign4.o process.o

assign4.o : assign4.cc
	g++ -Wall -c assign4.cc
process.o: process.cc
	g++ -Wall -c process.cc
clean :
	- rm *.o Assign4
