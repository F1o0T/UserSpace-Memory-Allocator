#include "runtime/FixedHeap.h"

//B = size of memory blocks
template <int B>
FixedHeap<B>::FixedHeap(FixedMemory<> fmemory)
{
        //this->memory = fmemory;
        this->blocklist = bool[(M/B)];
        for(bool elem: blocklist)
        {
                elem = 1;
        }
}

template <int B>
void* FixedHeap<B>::alloc(size_t size)
{
        //prueft ob die angeforderte Speichergroesse eine ganze Zahl als Anzahl der Bloecke ergibt
        if ((size % B) == 0) {
                int numberofblocks = size/B;
                int count = 0;
                
                //schaut in der Liste welche Speicherbloecke frei sind
                for (int i = 0; i < (memory.getSize())/B; i++) {
                        if (blocklist[i] == 1) {
                                count++;
                        } else {
                                count = 0;
                        }
                        
                        //falls die Anzahl an nebeneinander freien Bloecken erreicht ist, wird Pointer zurueck gegeben
                        if (count == numberofblocks) {
                                for (int j = i - numberofblocks; j < i; j++) {
                                        blocklist[i] = 0;                       //Block als nicht verfuegbar kennzeichnen
                                        void* ptr = (int*) memory.getStart() + ((i - numberofblocks) * B);             //pointer auf den Anfang des Blockabschnitts berechnen
                                        return ptr;
                                }
                        }
                }
        } else {
                return 0;
        }
}

template <int B>
void FixedHeap<B>::free(void* address)
{
	int place = (((int) address - (int) memory.getStart()))/B;
	blocklist[place] = 1;

}
