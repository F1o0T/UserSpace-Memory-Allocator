#include "../include/system/Memory.h"
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <list>
using namespace std;


template <size_t fixSize> class FixedMemory: public Memory{
    public:

        FixedMemory(){
            this->size = fixSize;
            this->start = malloc(this->size);

            printf("memory at ");
            printf("%p", getStart());
            printf(" with size of ");
            printf("%zu",getSize());
            printf(" was allocated\n");

        }

    void* expand(size_t size){
        return NULL;
    }
};

