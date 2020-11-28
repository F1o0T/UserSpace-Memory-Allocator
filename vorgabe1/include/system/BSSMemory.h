#ifndef BSSMemory_h
#define BSSMemory_h

#include "system/Memory.h"
#include <unistd.h>

#define stdMem 1024

class BSSMemory:public Memory {
public:
	BSSMemory() {
		this -> memblock = sbrk(0);
	}
	
	void* getStart() {
		return this -> memblock;
	}
	
	size_t getSize() {
		return this -> heapSize;
	}
	
	void* expand(size_t size) {
		if (size < stdMem) {
			this -> heapSize += stdMem;
			return sbrk(stdMem);
		} else {
			this -> heapSize += ((size/stdMem) + 1) * stdMem;
			return sbrk(((size/stdMem) + 1) * stdMem);
		}
	}
	
private:
	void* memblock;
	unsigned heapSize = 0;
};

#endif