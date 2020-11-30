#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "runtime/Heap.h"
#include "system/BSSMemory.h"
#include <vector>

using namespace std;

#define minByte sizeof(freeBlock)
#define sizeUnsi sizeof(unsigned)

struct freeBlock {
    unsigned freeSpace;//ist immer der frei Platz - 4 byte für die die groesse des Blockes
    freeBlock* nextAddress;
};

class FirstFitHeap:public Heap {
public:

    FirstFitHeap(Memory& memory);

    void initHeap(int n);

    void* alloc(size_t size);

	int getSize();

    void fillList(list<int>* list);

    void merge(freeBlock* block1, freeBlock* block2);

    void addBlockInList(freeBlock* block);

    void free(void* address);

private:
    freeBlock* head;
};

#endif