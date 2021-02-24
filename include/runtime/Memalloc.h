#include <iostream>
#include "runtime/FirstFitHeap.h"

using namespace std; 


//void* malloc(size_t size);
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void* malloc(size_t size);