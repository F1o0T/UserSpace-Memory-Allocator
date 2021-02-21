#include "runtime/MemAlloc.h"

freeBlock* FirstFitHeap::head = (freeBlock*) vMem.getStart();
bool FirstFitHeap::initalized = false;

//FirstFitHeap heap;

void* operator new(size_t size) {
    cout << "new is called with size = " << size << endl; 
    return FirstFitHeap::malloc(size);
}

void* operator new[](size_t size, int dummy) {
    cout << "new []" << endl; 
    return FirstFitHeap::malloc(size);
}

void operator delete(void* ptr) {
    FirstFitHeap::free(ptr);
}

void operator delete[](void* ptr) {
    FirstFitHeap::free(ptr);
}

void memAllocInitialize()
{
    FirstFitHeap::initHeap();
    cout << "Mmeory has been initalized" << endl;
}

void* malloc(size_t size)
{
    cout << "We are here in malloc in " << __FILE__ << endl;
    FirstFitHeap::vMem = VirtualMem();
    exit(1);
    void* returnPtr = FirstFitHeap::malloc(size); 
    return returnPtr;
}

void free(void *ptr)
{
    FirstFitHeap::free(ptr); 
}
