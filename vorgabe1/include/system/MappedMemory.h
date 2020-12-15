#ifndef MappedMemory_h
#define MappedMemory_h

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include "system/Memory.h"

#define pagesize sysconf(_SC_PAGESIZE) //4096

class MappedMemory:public Memory {
public:

	MappedMemory(unsigned int startSize);

	~MappedMemory();
	
	void* getStart();
	
	size_t getSize();
	
	void* expand(size_t size);
	
private:
	void* memblock;
	size_t length;
	int fd;
};

#endif