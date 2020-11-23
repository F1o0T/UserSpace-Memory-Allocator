#ifndef FixedHeap_h
#define FixedHeap_h

#include <iostream>
#include "runtime/Heap.h"
#include "system/FixedMemory.h"
#include <vector>

using namespace std;

template <int N = 1>
class FixedHeap:public Heap {
public:
	
	FixedHeap(Memory& memory) : Heap(memory) {
		if (N <= 0) {
			cerr << "Error: Die Blockgroesse darf nicht 0 sein." << endl;

		} else if (memory.getSize() <= 0) {
			cerr << "Error: Memory darf nicht 0 sein" << endl;

		} else if (N > memory.getSize()) {
			cerr << "Error: Die Blockgroesse darf nicht groesser als Memory sein" << endl;

		} else {
			int blocklistlength = (int) (2*(memory.getSize()/N));
			blocklistlength--;

			for (int i = 0; i < blocklistlength; i++) {
				blocklist.push_back(false);
			}
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
		if (getSize()) {
			int numberofblocks = 0;
			
			if (size != 0) {
				numberofblocks = size/N;
			} else {
				cerr << "Error: Bitte keine 0 eingeben!" << endl;
				return nullptr;
			}
			
			//kein vollständiger Block bekommt trotzdem ein Block
			if ((size % N) != 0) {
				numberofblocks++;
			}
			
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
					
					return ((caddr_t) memory.getStart()) + ((i/2) * N);
				}
			}
			
			cerr << "Error: Du hast keinen Platz mehr im Heap." << endl;

		} else {
			cerr << "Error: Heap ist fehlerhaft!" << endl;
		}

		return nullptr;
	}
	

	void free(void* address) {
		if (getSize()) {
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
				if (count > getSize()-1) {
					cerr << "Die Adresse ist kein Block im Heap." << endl;
					return;
				} else if (blocklist[count-1] == 1) {
					cerr << "Bitte gib das erste Element des Speicherblockes ein!" << endl;
					return;
				}
			}
			
			//finden aller zusammenhängender Blöcke
			while (blocklist[count] == 1 && count <= getSize()-1) {
				blocklist[count] = 0;
				count++;
			}
		} else {
			cerr << "Error: Heap ist fehlerhaft!" << endl;
		}
	}
	
private:
	vector<bool> blocklist;
};

#endif
