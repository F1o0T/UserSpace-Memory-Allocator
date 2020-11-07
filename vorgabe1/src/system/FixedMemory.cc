#include "system/FixedMemory.h"

template <int N>
void* FixedMemory<N>::getStart() {
	return this -> memblock;
}

template <int N>
size_t FixedMemory<N>::getSize() {
	return (size_t) N;
}

template <int N>
void* FixedMemory<N>::expand(size_t size) {
	return 0;
}
