#include "system/BSSMemory.h"

BSSMemory::BSSMemory(unsigned int startSize) {
    initMem(startSize);
}



void BSSMemory::initMem(unsigned startSize) {
    //16 = Mindestgroesse wegen der Informationen
    if(startSize > 16){
        this ->expandSize = startSize;
    } else{
        this ->expandSize = 16;
    }
    this -> memblock = sbrk(expandSize);
}

void* BSSMemory::getStart() {
    return this -> memblock;
}

size_t BSSMemory::getSize() {
    return (size_t) (((char*)sbrk(0)) - ((char*) memblock));
}

void* BSSMemory::expand(size_t size) {
    if (size <= expandSize) {
        return sbrk(expandSize);
    } else {
        return sbrk(((size/expandSize) + 1) * expandSize);
    }
}