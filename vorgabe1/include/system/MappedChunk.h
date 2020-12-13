#ifndef MappedChunk_h
#define MappedChunk_h

#include "system/Memory.h"
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;


class MappedChunk:public Memory {
public:

	MappedChunk(size_t startSize);

	MappedChunk(size_t startSize, size_t blocks, size_t maxactBlocks);

	~MappedChunk();
	
	void* getStart();
	
	size_t getSize();

	void* activateChunk();

	void deactivateChunk(void* chunkStart);
	
	void* expand(size_t size);
	
private:
	void* memblock;
	size_t chunks;		//number of blocks the momory is devided into
	size_t maxactChunks;
	size_t chunkSize;
	vector<bool> chunkList;

};

#endif