#ifndef FixedMemory_h
#define FixedMemory_h

#include "system/Memory.h"

template <int M>
class FixedMemory:public Memory {
public:
	FixedMemory() {}

	void initMem() {
		this -> memblock = malloc(M);
	}

	~FixedMemory(){
		free(this -> memblock);
	}
	
	void* getStart() {
		return this -> memblock;
	}
	
	size_t getSize() {
		return (size_t) M;
	}
	
	void* expand(size_t size) {
		return (void*) -1;
	}
	
private:
	void* memblock;
};

#endif
