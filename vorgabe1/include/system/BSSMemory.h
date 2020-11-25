#ifndef BSSMemory_h
#define BSSMemory_h

#include "system/Memory.h"
#include <unistd.h>

class BSSMemory:public Memory {
public:
	BSSMemory() {
		this -> memblock = sbrk(0);
		//expand(1024);//maybe gleich oder erst nach ersten Aufruf
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
		//vielleicht hier eine groesseres Size waehlen um nicht alle 2 sec expand aufzurufen
		return sbrk(size);
	}
	
private:
	void* memblock;
};

#endif