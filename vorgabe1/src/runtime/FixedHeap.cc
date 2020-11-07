#include "../system/FixedMemory.cc"
#include "runtime/Heap.h"
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>


template <size_t blockSize> class FixedHeap: public Heap
{
    public:
        
        FixedHeap(FixedMemory fMem)
        {
           this->memory = fMem;
           this->freeBlocks = new list <int>();
           initBlockList();
        }

        void* alloc(size_t size)
        {
                return this->memory.alloc(size_t);
        }

        void free(void* address)
        {
                this->memory.free(address);
        }
        
        
        
    private:

        void initBlockList(){
                this-> freeBlocks = new list void*();

                for(int i = 0;i+blockSize; i < this->memory.getSize())
                {
                        freeBlocks.push_back((int)memory.getStart() + i);
                }
        }

        list <int> freeBlocks;
    
};
