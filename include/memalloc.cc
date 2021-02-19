#include <iostream>
#include <fstream>
#include "system/VirtualMem.h"
#include "runtime/FirstFitHeap.h"
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"
using namespace std; 

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
    void* returnPtr = FirstFitHeap::malloc(size); 
    return returnPtr;
}

void free(void *ptr)
{
    FirstFitHeap::free(ptr); 
}
