#ifndef FixedMemory_h
#define FixedMemory_h

#include "system/Memory.h"

template <int M>
class FixedMemory:public Memory {
public:
	FixedMemory();
	
	void* getStart();
	
	size_t getSize();
	
	void* expand(size_t size);
	
private:
	int memblock[M];
};

#endif
