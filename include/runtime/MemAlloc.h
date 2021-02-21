#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H
#include <iostream>
#include "runtime/FirstFitHeap.h"

void* operator new(size_t size);

void* operator new[](size_t size, int dummy);

void operator delete(void* ptr);

void operator delete[](void* ptr);


namespace memAlloc
{
void* malloc(size_t size);

void free(void *ptr);
}
#endif
