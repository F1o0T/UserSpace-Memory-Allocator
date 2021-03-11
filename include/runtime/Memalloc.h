#include <iostream>
#include "runtime/FirstFitHeap.h"

using namespace std; 


void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void* malloc(size_t size);
void *realloc(void* ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);