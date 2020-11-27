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
/*
	~BSSMemory(){
		brk(memblock);
	}*/
	
	void* getStart() {
		return this -> memblock;
	}
	
	size_t getSize() {
		return (size_t) ((char*) sbrk(0) - (char*) memblock);
	}
	
	void* expand(size_t size) {
		if (size < stdMem) {
			return sbrk(stdMem);
		} else {
			return sbrk((size/stdMem) * stdMem);
		}
	}
	
private:
	void* memblock;
};

#endif