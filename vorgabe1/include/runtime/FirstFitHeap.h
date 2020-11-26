#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "runtime/Heap.h"
#include "system/BSSMemory.h"
#include <vector>

using namespace std;

struct nextFreeUnit {
    nextFreeUnit* nextAddress;
    size_t freeSpace;
    bool tail;
};

class FirstFitHeap:public Heap {
public:

    FirstFitHeap(Memory& memory, size_t n) : Heap(memory) {
        if (n <= 0) {
			cerr << "Error: Die Heapgroesse darf nicht 0 sein." << endl;
            //Achtung brauchen neuen Fehlerzustand blocklist ist nicht mehr
		} else {
            memory.expand(n);
            initHeap(memory.getSize(), (nextFreeUnit*) memory.getStart());
        }
    }

    void initHeap(size_t length, nextFreeUnit* start) {
        size_t length = memory.getSize();
        nextFreeUnit* first = (nextFreeUnit*) memory.getStart();

        this -> head = first;
        first -> nextAddress = 0;
        first -> freeSpace = length;
        first -> tail = true;

        //erste direkte Liste angelegt an den Anfang von Memory mit ein free-Block mit Laenge length
        //muss speichern next (adresse = 8 byte) und wie gross (int = 4 byte) 12 byte pro Block?!!!
    }

    void* alloc(size_t size) {//??? muessen alloc und free komplett neu machen
        if (size == 0) {
            cerr << "Error: Bitte keine 0 eingeben!" << endl;
            return nullptr;
        }
        
        nextFreeUnit* rightPos = 0;
        nextFreeUnit* curPos = this -> head;

        do {
            if ((size_t) (curPos -> freeSpace) >= size) {
                rightPos = curPos;
            } else if (curPos -> nextAddress == 0) {
                void* ptr = memory.expand(size);
                //neuer Free-Block muss beschrieben werden

            }


            //ein Listeneintrag weitergehen
        } while (rightPos == 0);

        //Block als belegt makieren
        
        return rightPos;

        /*int count = 0;
        
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
        }*/

    }

    void merge(void* deletedBlock) {
        //verbinden von mindestens zwei Blöcken
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
            if (count > getSize()-1) {
                cerr << "Error: Die Adresse ist kein Block im Heap." << endl;
                return;
            } else if (blocklist[count-1] == 1) {
                cerr << "Error: Bitte gib das erste Element des Speicherblockes ein!" << endl;
                return;
            }
        }
        
        //finden aller zusammenhängender Blöcke
        while (blocklist[count] == 1 && count <= getSize()-1) {
            blocklist[count] = 0;
            count++;
        }
    }

private:
    nextFreeUnit* head;

};

#endif