#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "system/VirtualMem.h"
#include <vector>

using namespace std;

#define minByte sizeof(freeBlock)
#define sizeUnsi sizeof(unsigned)

class freeBlock {
public:
    unsigned freeSpace;//ist immer der frei Platz - 4 byte für die die groesse des Blockes
    freeBlock* nextAddress = NULL;
};

void signalHandler(int sigNUmber, siginfo_t *info, void *ucontext);

class FirstFitHeap {
public:

    FirstFitHeap();
    ~FirstFitHeap();

    void destroyTimer();

    void initHeap();

    void* malloc(size_t size);
    
	void* realloc(void* ptr, size_t size);

	void* calloc(size_t nmemb, size_t size);

    void free(void* address);

    size_t setRightSize(freeBlock *memBlock, size_t size);


    void* operator new(size_t size);

    void* operator new[](size_t size);

    void operator delete(void* ptr);

    void operator delete[](void* ptr);

	int getSize();
   
private:
    
    void fillList(list<int>* list);
    void merge(freeBlock* block1, freeBlock* block2);
    void addBlockInList(freeBlock* block);
    bool correctAddress(void* address);
    
    struct sigaction SigAction;
    freeBlock* head;


};

extern FirstFitHeap heap;

#endif