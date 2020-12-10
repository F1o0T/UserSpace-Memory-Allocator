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
			cerr << "Error: The block size should not be 0." << endl;

		} else if (memory.getSize() <= 0) {
			cerr << "Error: The memory should not be 0." << endl;

		} else if (N > memory.getSize()) {
			cerr << "Error: The block size should not be greater than memory. " << endl;

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
	
    void fillList(list<int>* list) {      
		bool col = true;  
		
		for (int i = 0; i < getSize(); i += 2) {
		    if (blocklist[i]) {
			    if (i != 0) {
				    if (!(blocklist[i-1])) {
					    col = !col;
				    }
			    }
			
			    if (col) {
					list -> push_back(-1);
			    } else {
					list -> push_back(-2);
			    }
			
			} else {
				list -> push_back(-3);
			}
		}
    }
	

	//even -- false(0) = free blocks -- true(1) = used blocks
	//odd -- false(0) = no relation -- true(1) = relation between blocks
	void* alloc(size_t size) {
		if (getSize()) {
			int numberofblocks = 0;
			
			if (size != 0) {
				numberofblocks = size/N;
			} else {
				cerr << "Error: Please dont use a 0!" << endl;
				return nullptr;
			}
			
			//not complete block but still gets a complete block
			if ((size % N) != 0) {
				numberofblocks++;
			}
			
			int count = 0;
			
			//Searching for first fitting gap
			for (int i = 0; i < getSize(); i += 2) {
				if (blocklist[i] == 0) {
					count++;
				} else {
					count = 0;
				}
				
				//Gap was found else error
				if (count == numberofblocks) {
					count *= 2;
					i = (i+2) - count;
					
					//set all related blocks to 1
					for (int j = i; j < i + (count-1); j++) {
						blocklist[j] = 1;
					}
					
					return ((caddr_t) memory.getStart()) + ((i/2) * N);
				}
			}
			
			cerr << "Error: Your heap has not enough space left." << endl;

		} else {
			cerr << "Error: Your heap is corrupted!" << endl;
		}

		return nullptr;
	}
	

	void free(void* address) {
		if (getSize()) {
			char* start = (char*) (memory.getStart());
			char* obj = (char*) address;
			int count = 0;
			
			if (start != obj) {
				//locate the given block
				for (int i = 0; i <= getBlockCount(); i++) {
					
					//make a step from block to block
					if (start == obj) {
						break;
					}
					
					start += N;
					count += 2;
				}
				
				//backtracking and debugging (simple)
				
				//If no matching block was found or it isn't the first element
				if (count > getSize()-1) {
					cerr << "Error: The address doesn't exist in the heap." << endl;
					return;
				} else if (blocklist[count-1] == 1) {
					cerr << "Error: Please enter the first element of the memory blocks!" << endl;
					return;
				}
			}
			
			//find all related blocks
			while (blocklist[count] == 1 && count <= getSize()-1) {
				blocklist[count] = 0;
				count++;
			}
		} else {
			cerr << "Error: Your heap is corrupted!" << endl;
		}
	}
	
private:
	vector<bool> blocklist;
};

#endif
