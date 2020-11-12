#include "system/FixedMemory.h"

template <int M>
FixedMemory<M>::FixedMemory() {
	
}

template <int M>
void* FixedMemory<M>::getStart() {
	return this -> memblock;
}

template <int M>
size_t FixedMemory<M>::getSize() {
	return (size_t) M;
}

template <int M>
void* FixedMemory<M>::expand(size_t size) {
	return 0;
}
