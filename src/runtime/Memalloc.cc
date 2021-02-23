#include "runtime/Memalloc.h"

VirtualMem e_vMem;
VirtualMem* FirstFitHeap::vMem = &e_vMem;
freeBlock* FirstFitHeap::head = (freeBlock*) e_vMem.getStart();
FirstFitHeap heap;

/*
void *malloc(size_t size){
    return FirstFitHeap::malloc(size);
}
*/

void* operator new(size_t size) {
    cout << "own new, size: " << size <<endl;
    return FirstFitHeap::malloc(size);
}

void* operator new[](size_t size) {
    cout << "own new[], size: " << size <<endl;
    return FirstFitHeap::malloc(size);
}

void operator delete(void* ptr) {
    FirstFitHeap::free(ptr);
}

void operator delete[](void* ptr) {
    FirstFitHeap::free(ptr);
}

/*
void* malloc(size_t size)
{
    return FirstFitHeap::malloc(size);
}
*/

