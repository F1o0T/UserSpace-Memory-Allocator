#ifndef MappedChunk_h
#define MappedChunk_h

#include "system/Memory.h"
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;


class MappedChunk{
public:
	/////////////////////////////////////////////////
	// Signal handeler, constructor and deconstructor. 
	static void SignalHandeler(int SigNumber); 
	MappedChunk(size_t startSize, size_t blocks, size_t maxactBlocks);
	~MappedChunk();
	/////////////////////////////////////////////////
	// Basic Methods
	void* getStart();
	size_t getSize();
 	void* expand(size_t size);
 	/////////////////////////////////////////////////
 	// Advanced Methods
 	void FixPermissions();
	void MapSomeoneOut();
	void printChunkStarts(); 
	/////////////////////////////////////////////////
private:
	void* memblock = NULL;
	size_t chunksNumber = 0;
	size_t maxActChunks = 0;
	size_t chunkSize;
	vector<bool> chunksTable;

};

#endif