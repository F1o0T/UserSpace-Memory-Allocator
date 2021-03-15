#include "runtime/Memalloc.h"


VirtualMem vMem;
FirstFitHeap heap;
std::mutex myMutex;


void* operator new(size_t size) {
    cout << "own new" << endl;

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
    void* ptr = heap.malloc(size);
    return ptr;
}

void *realloc(void* ptr, size_t size){
    cout << "own realloc" << endl;

    void *pointer =  heap.realloc(ptr, size);

    return pointer;
}

void *calloc(size_t nmemb, size_t size){

    void *ptr = heap.calloc(nmemb, size);

    return ptr;
}

void free(void* address) {
    heap.free(address);

}


