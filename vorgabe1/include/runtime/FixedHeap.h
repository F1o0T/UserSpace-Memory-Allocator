#ifndef FixedHeap_h
#define FixedHeap_h

#include "runtime/Heap.h"
#include "system/FixedMemory.h"
#include <vector>

template <int N>
class FixedHeap:public Heap {

public:
	
	
	FixedHeap(Memory& memory) : Heap(memory) {
		int blocklistlength = (int) (2*(memory.getSize()/N));
		blocklistlength--;
		
		for (int i = 0; i < blocklistlength; i++) {
			blocklist.push_back(false);
		}
	}
	
	int getSize() {
		return (int) blocklist.size();
	}
	
	int getBlockCount() {
		return (blocklist.size() + 1) / 2;
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
			
			//suchen der ersten passenden Spalte
			for (int i = 0; i < getSize(); i += 2) {
				if (blocklist[i] == 0) {
					count++;
				} else {
					count = 0;
				}
				
				//Spalte wurde gefunden, sonst error
				if (count == numberofblocks) {
					count *= 2;

					i = (i+2) - count;
					
					//alle zugehörigen Blöcke auf 1 stellen
					for (int j = i; j < i + (count-1); j++) {
						blocklist[j] = 1;
					}
					
					return ((char*) memory.getStart()) + ((i/2) * N);
				}
			}
		}
		
		return nullptr;

	}
	
	void free(void* address) {
		char* start = (char*) (memory.getStart());
		char* obj = (char*) address;
		int count = 0;
		
		if (start != obj) {
			//finden des angegebenen Blockes
			for (int i = 0; i <= getBlockCount(); i++) {
				
				//gehen Blockweise durch
				if (start == obj) {
					break;
				}
				
				start += N;
				count += 2;
			}
			
			//backtracken und Fehlersuche (simpel gehalten)
			
			//wenn kein passender Block gefunden wurde oder nicht das erste Element ist
			if (count > getSize()-1 || blocklist[count-1] == 1) {
				//Error
				return;
			}
		}
		
		//finden aller zusammenhängender Blöcke
		while (blocklist[count] == 1 && count <= getSize()-1) {
			blocklist[count] = 0;
			count++;
		}
	}

	size_t freeBlocks(){
		size_t count = 0;
		for(int i=0; i < blocklist.size(); i++){
			if(i % 2 == 0){
				if(blocklist[i] == 0){
					count++;
				}
			}
		}
		return count;
	}

	

private:
	vector<bool> blocklist;
};

#endif
