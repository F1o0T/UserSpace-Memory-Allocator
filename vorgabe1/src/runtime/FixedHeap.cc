#include "runtime/FixedHeap.h"

template <int N>
void* FixedHeap<N>::alloc(size_t size) {//wozu size sollte nicht ein Block == N sein?
	if ((size % N) == 0) {
		int numberofblocks = size/N;
		int count = 0;
		
		for (int i = 0; i < (memory.getSize())/N; i++) {
			if (blocklist[i] == 1) {
				count++;
			} else {
				count = 0;
			}
			
			if (count == numberofblocks) {
				for (int j = i - numberofblocks; j < i; j++) {
					blocklist[i] = 0;
					void* ptr = (int*) memory.getStart() + ((i - numberofblocks) * N);
					return ptr;
				}
			}
		}
	} else {
		return 0;
	}
}

template <int N>
void FixedHeap<N>::free(void* address) {//wie viel soll ich frei geben ein Block oder soviele Teile wie bei alloc beantragt wurden
	int place = (((int) address - (int) memory.getStart()))/N;
	blocklist[place] = 1;
}
