#include "runtime/Memalloc.h"


VirtualMem vMem;
FirstFitHeap heap;


void* operator new(size_t size) {
    return heap.malloc(size);
}

void* operator new[](size_t size) {
    return heap.malloc(size);
}

void operator delete(void* ptr) {
    heap.free(ptr);
}

void operator delete[](void* ptr) {
    heap.free(ptr);
}


void* malloc(size_t size)
{
    vMem.stopTimer();
    return heap.malloc(size);
    vMem.startTimer();
}

void *realloc(void* ptr, size_t size){
    vMem.stopTimer();
    return heap.realloc(ptr, size);
    vMem.startTimer();
}

void *calloc(size_t nmemb, size_t size){
    vMem.stopTimer();
    return heap.calloc(nmemb, size);
    vMem.startTimer();
}

void free(void* address) {
    vMem.stopTimer();
    heap.free(address);
    vMem.startTimer();
}


