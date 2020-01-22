#include <list>
#include <string>
#include <cstdlib>
#include "memory.cpp"

using namespace std;

/*
this is where:
- the HOWOFTEN constant will be defined
- the 2 linked lists of blocks are created, Avail and InUse
*/

#define HOWOFTEN 5//used to know when to reprint the contents of the both lists
#define MB 1048576//how many bytes that are in a MB


list<Block> Avil;
list<Block> InUse;

//prototypes
void bRun();//Best-Fit
void fRun();//First-Fit
void terminate(string);
void insert(Block);
void deallocate(string);
void bLoad(string);//Best-Fit
void fLoad(string);//First-Fit
void print();
