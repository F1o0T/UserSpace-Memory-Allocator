#ifndef FixedMemory_h
#define FixedMemory_h

#include "system/Memory.h"

template <int M>
class FixedMemory:public Memory {
public:
<<<<<<< HEAD
	//M= size of Memory
	//template <int M>
	FixedMemory(){
		this->memsize = M;
		start = malloc(M);
	}


	//template <int M>
	void* getStart() {
		return this -> start;
	}

	
	//template <int M>
		size_t getSize() {
		return this -> memsize;
	}
	
	//template <int M>
		void* expand(size_t size) {
=======
	FixedMemory() {
		this -> memblock = malloc(M);
	}
	
	void* getStart() {
		return this -> memblock;
	}
	
	size_t getSize() {
		return (size_t) M;
	}
	
	void* expand(size_t size) {
>>>>>>> dustin
		return 0;
	}
	
private:
<<<<<<< HEAD
	void* start;
	size_t memsize;
=======
	void* memblock;
>>>>>>> dustin
};

#endif
