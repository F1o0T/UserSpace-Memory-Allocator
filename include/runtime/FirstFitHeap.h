#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "system/VirtualMem.h"
#include <vector>

using namespace std;

#define minByte sizeof(freeBlock)
#define sizeUnsi sizeof(unsigned)

struct freeBlock {
    unsigned freeSpace;//ist immer der frei Platz - 4 byte für die die groesse des Blockes
    freeBlock* nextAddress = NULL;
};

class FirstFitHeap {
public:

    FirstFitHeap();
    ~FirstFitHeap();

    void destroyTimer();

    static void signalHandler(int sigNUmber, siginfo_t *info, void *ucontext);

    static void initHeap();

    static void* malloc(size_t size);
    
	static void* realloc(void* ptr, size_t size);

	static void* calloc(size_t nmemb, size_t size);

    static void free(void* address);


    void* operator new(size_t size);

    void* operator new[](size_t size);

    void operator delete(void* ptr);

    void operator delete[](void* ptr);

	int getSize();

    //static VirtualMem* vMem;

    static freeBlock* head;

   
private:
    
    void fillList(list<int>* list);

    static void merge(freeBlock* block1, freeBlock* block2);

    static void addBlockInList(freeBlock* block);

    static bool correctAddress(void* address);


};

#endif