#include "system/BSSMemory.h"

BSSMemory::BSSMemory() {}

void BSSMemory::initMem() {
    this -> memblock = sbrk(0);
}

void* BSSMemory::getStart() {
    return this -> memblock;
}

size_t BSSMemory::getSize() {
    return (size_t) (((char*)sbrk(0)) - ((char*) memblock));
}

void* BSSMemory::expand(size_t size) {
    if (size <= stdMem) {
        return sbrk(stdMem);
    } else {
        return sbrk(((size/stdMem) + 1) * stdMem);
    }
}