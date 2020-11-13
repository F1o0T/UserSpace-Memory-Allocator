#ifndef FixedHeap_h
#define FixedHeap_h

#include "runtime/Heap.h"
#include "system/FixedMemory.h"

template <int N>
class FixedHeap:public Heap {
public:
	
	FixedHeap(Memory& memory) : Heap(memory) {
		bool* array = (bool*) calloc(2*(memory.getSize()/N), sizeof(bool));
		if (array == NULL) {/* noch was machen*/}
		this -> blocklist = array;
	}
	
	int getList() {
		return sizeof(blocklist[5]);
	}
	
	bool getList(int i) {
		return blocklist[i];
	}
	
	//gerade -- false(0) = freie Blöcke -- true(1) = belegte Blöcke
	//ungerade -- false(0) = keine Beziehung -- true(1) = Beziehung
	void* alloc(size_t size) {
		if ((size % N) == 0) {
			int numberofblocks = size/N;
			int count = 0;
			
			for (int i = 0; i < (int) (2*((memory.getSize())/N)); i+2) {
				if (blocklist[i] == 0) {
					count++;
				} else {
					count = 0;
				}
				
				if (count == numberofblocks) {
					count *= 2;
					i = (i+2) - count;
					for (int j = i; j < i + (count-1); j++) {
						blocklist[j] = 1;
					}
					
					return memory.getStart() + ((i/2) * N);
				}
			}
		}
		
		return nullptr;
	}
	
	void free(void* address) {
	}
	
private:
	bool* blocklist;
};

#endif
