#ifndef FixedMemory_h
#define FixedMemory_h

#include "system/Memory.h"

template <int M>
class FixedMemory:public Memory {
public:
	FixedMemory() {
		this -> memblock = malloc(M);
	}
	
	void* getStart() {
		return this -> memblock;
	}
	
	size_t getSize() {
		return (size_t) M;
	}
	
	void* expand(size_t size) {
		return 0;
	}
	
private:
	void* memblock;
};

#endif
