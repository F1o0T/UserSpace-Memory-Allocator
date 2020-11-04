#include "../include/system/Memory.h"
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
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

    //void* Memory::getStart(){
    //    return this->start;
    //}

    //size_t Memory::getSize(){
    //    return this->size;
    //}

    void* expand(size_t size){
        return 0;
    }
};

int main(){

    using namespace std;
    FixedMemory<1024> mem;
    
    return 1;
}

