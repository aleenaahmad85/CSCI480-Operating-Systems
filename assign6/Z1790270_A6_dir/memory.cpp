/*this file will define a class/struct to represent one block of memory

include:
-starting address, int
-size, int
-process ID of the owner, string might be blank
-ID of the block, string might be blank

*/
#include <string>
#include <iomanip>
#include <iostream>

using namespace std;

class Block
{
	public:
		Block(int newSize, int address)
		{
			size = newSize;
			startAddress = address;
		}
		int startAddress;
		int size;
		int pid;
		string name;
		Block * previous = NULL;
		Block * next = NULL;
};
