#ifndef FixedHeap_h
#define FixedHeap_h

#include "runtime/Heap.h"
#include "system/FixedMemory.h"

template <int N>
class FixedHeap:public Heap {
public:
	
	FixedHeap() {
		//bool list[M/N] = {1};
		//this -> blocklist = list;
	}
	
	void* alloc(size_t size);
	
	void free(void* address);
	
private:
	bool blocklist[];
};

#endif
