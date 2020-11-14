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

		for(int i = 0; i < blocklistlength; i++){//Schleife um Startwerte zu belegen
				blocklist.push_back(false);
		}
	}
	

	int getBlockCount() {
		return blocklist.size()/2;
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
			
			for (int i = 0; i < blocklist.size(); i+=2) {
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
					
					return ((char*) memory.getStart()) + ((i/2) / N);
				}
			}
		}
		
		return nullptr;

	}
	
	void free(void* address) {
		cout << address << endl;
		cout << memory.getStart() << endl;
		char* start = (char*) (memory.getStart());
		char* dest = (char*) address;
		int count = 0;
		while(start != dest){
			start = start + 1; //um ein Byte erhöhen
			count +=1;  		//CounterAnzahl der erhöhten Bytes
		}
		count = count/N; //Umrechnung in Anzahl Bloecke
		blocklist[count] = 0;
		while (blocklist[count +1] == 1){
			blocklist[count +1] = 0;		//setzt Beziehung falls vorhanden auf 0
			count += 2;
			blocklist[count] = 0;			//setzt Block wieder auf frei
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
