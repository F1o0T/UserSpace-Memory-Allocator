#include "system/FixedMemory.h"

//M= size of Memory
template <int M>
FixedMemory<M>:: FixedMemory(){
    this->size = M;
    this->start = malloc(M);
}

template <int M>
void* FixedMemory<M>::getStart() {
	return this -> memblock;
}

template <int M>
size_t FixedMemory<M>::getSize() {
	return this -> memsize;
}

template <int M>
void* FixedMemory<M>::expand(size_t size) {
	return 0;
}


