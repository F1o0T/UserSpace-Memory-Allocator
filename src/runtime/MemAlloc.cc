#include "runtime/MemAlloc.h"

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

namespace memAlloc
{
    void* malloc(size_t size)
    {
        cout << "Here" << endl;
        void* returnPtr = FirstFitHeap::malloc(size); 
        return returnPtr;
    }

    void free(void *ptr)
    {
        FirstFitHeap::free(ptr); 
    }
}