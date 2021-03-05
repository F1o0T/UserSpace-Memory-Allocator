#include "runtime/Memalloc.h"


extern FirstFitHeap heap;

/*
void *malloc(size_t size){
    return heap.malloc(size);
}
*/

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
    return heap.malloc(size);
}

void *realloc(void* ptr, size_t size){
    return heap.realloc(ptr, size);
}

void *calloc(size_t nmemb, size_t size){
    return heap.calloc(nmemb, size);
}

void free(void* address) {
    heap.free(address);
}


