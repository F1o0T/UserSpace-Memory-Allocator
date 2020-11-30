#ifndef BSSMemory_h
#define BSSMemory_h

#include "system/Memory.h"
#include <unistd.h>

#define stdMem 1024

class BSSMemory:public Memory {
public:

	BSSMemory();

	void initMem();
	
	void* getStart();
	
	size_t getSize();
	
	void* expand(size_t size);
	
private:
	void* memblock;
};

#endif