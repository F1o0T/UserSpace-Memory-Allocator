#ifndef FixedHeap_h
#define FixedHeap_h

#include "runtime/Heap.h"
#include "system/FixedMemory.h"
#include <vector>

template <int N>
class FixedHeap:public Heap {

public:
	
	
	FixedHeap(Memory& memory) : Heap(memory) {
		int blocklistlength = (int) (memory.getSize()/N);
		blocklistlength *=2;

		for(int i = 0; i < blocklistlength; i++){//Schleife um abwechselnd true und false einzufuegen
			if(blocklistlength % 2 == 0){
				blocklist.push_back(true);
			}else{
				blocklist.push_back(false);
			}
		}
	}
	

	int getListSize() {
		return blocklist.size();
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
			
			for (int i = 0; i < blocklist.size(); i+=8) {
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
		int pos = (char*) (memory.getStart()) - (char*) address;//Abstand in Bytes;
		pos = pos/N;//Abstand der Bloecke
		blocklist[pos] = 1;
		while (blocklist[pos +1] == 1){
			blocklist[pos +1] = 0;		//setzt Beziehung falls vorhanden auf 0
			pos += 2;
			blocklist[pos] = 1;			//setzt Block wieder auf 1
		}

	}
	

private:
	vector<bool> blocklist;

};

#endif
