#include "runtime/Memalloc.h"




/*
void *malloc(size_t size){
    return FirstFitHeap::malloc(size);
}
*/

void* operator new(size_t size) {
    //cout << "own new, size: " << size <<endl;
    return FirstFitHeap::malloc(size);
}

void* operator new[](size_t size) {
    //cout << "own new[], size: " << size <<endl;
    return FirstFitHeap::malloc(size);
}

void operator delete(void* ptr) {
    FirstFitHeap::free(ptr);
}

void operator delete[](void* ptr) {
    FirstFitHeap::free(ptr);
}


void* malloc(size_t size)
{
    return FirstFitHeap::malloc(size);
}
