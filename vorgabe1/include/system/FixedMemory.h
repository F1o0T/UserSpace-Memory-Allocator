#ifndef FixedMemory_h
#define FixedMemory_h

#include "system/Memory.h"

template <int N>
class FixedMemory:public Memory {
public:
	void* getStart();
	
	size_t getSize();
	
	void* expand(size_t size);
	
private:
	int memblock[N];
};

#endif
