#include "FixedHeap.cc"
#include "../include/runtime/Heap.h"
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>




template <size_t size> class FixedHeap: public Heap
{
    public:
        
        Fixedheap(FixedMemory fMem)
        {
           this->memory = fmem 
        }
        
        
        
private:
        size_t blockSize = size;
    
}
