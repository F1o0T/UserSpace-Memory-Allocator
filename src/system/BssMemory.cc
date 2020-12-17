#include "system/BSSMemory.h"

BSSMemory::BSSMemory(unsigned startSize) {
    //16 = minimum of size (because the heap struct)
    if(startSize > 16){
        this ->expandSize = startSize;
    } else{
        this ->expandSize = 16;
    }
}

void BSSMemory::initMem() {
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