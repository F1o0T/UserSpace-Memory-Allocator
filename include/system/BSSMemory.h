#ifndef BSSMemory_h
#define BSSMemory_h

#include "system/Memory.h"
#include <unistd.h>

//#define stdMem 1024

class BSSMemory:public Memory {
public:

	BSSMemory(unsigned int startSize);

	void initMem(unsigned startSize);
	
	void* getStart();
	
	size_t getSize();
	
	void* expand(size_t size);
	
private:
	void* memblock;
	unsigned int expandSize;
};

#endif