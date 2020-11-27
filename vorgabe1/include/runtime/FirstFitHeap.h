#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "runtime/Heap.h"
#include "system/BSSMemory.h"
#include <vector>

using namespace std;

struct nextFreeUnit {
    bool tail;//true wenn es der Tail von gesamten Heap ist(also auch belegte Bloecke)
    unsigned freeSpace;
    nextFreeUnit* nextAddress;
};

class FirstFitHeap:public Heap {
public:

    FirstFitHeap(Memory& memory, size_t n) : Heap(memory) {
        if (n <= 0) {
			cerr << "Error: Die Heapgroesse darf nicht 0 sein." << endl;
            //Achtung brauchen neuen Fehlerzustand blocklist ist nicht mehr
		} else {
            memory.expand(n);
            initHeap();
        }
    }

    void initHeap() {
        unsigned length = (unsigned) memory.getSize();
        nextFreeUnit* first = (nextFreeUnit*) memory.getStart();

        this -> head = first;
        first -> tail = true;
        first -> freeSpace = length;
        first -> nextAddress = 0;

        //erste direkte Liste angelegt an den Anfang von Memory mit ein free-Block mit Laenge length
    }

    void* alloc(size_t size) {
        if (size == 0) {
            cerr << "Error: Bitte keine 0 eingeben!" << endl;
            return nullptr;
        }
        
        unsigned* rightPos = 0;
        nextFreeUnit* lastPos = 0;
        nextFreeUnit* curPos = this -> head;

        while ((size_t) (curPos -> freeSpace) < size) {

            if (curPos -> nextAddress == 0) {
                memory.expand(size);

                if (curPos -> tail) {
                    curPos -> freeSpace += 1024;
                    //Problem was wenn mehr als 1024 angefragt wurde (dann wurde zwar mehr erweitert aber wir wissen nicht um wie viel)

                } else {
                    lastPos = curPos;
                    curPos = curPos -> nextAddress;

                    curPos -> tail = true;
                    curPos -> freeSpace = 1024;
                    curPos -> nextAddress = 0;
                }

                break;
            }

            lastPos = curPos;
            curPos = curPos -> nextAddress;
        }

        //right Posi wird nicht gebraucht brauchen aber letzte Position
        rightPos = (unsigned*) curPos;//zum gleich belegten Block der Pointer (koennte auch ein unsigned sein dann weniger casts)

        //brauch drei Pointer einer zum Block davor einen zu gleich belegten Block und einer zu naechsten
        //verschieben des alten Managments
        //Problem könnte belegt sein
        char* ar_ptr = (char*) rightPos;//zum naechsten Block der Pointer
        ar_ptr += sizeof(unsigned);
        ar_ptr += size;

        if (this -> head == curPos) {
            this -> head = ((nextFreeUnit*) ar_ptr);
        } else {
            lastPos -> nextAddress = ((nextFreeUnit*) ar_ptr);
        }

        ((nextFreeUnit*) ar_ptr) -> tail = ((nextFreeUnit*) rightPos) -> tail;
        ((nextFreeUnit*) ar_ptr) -> freeSpace = (((nextFreeUnit*) rightPos) -> freeSpace) - size - sizeof(unsigned);
        ((nextFreeUnit*) ar_ptr) -> nextAddress = ((nextFreeUnit*) rightPos) -> nextAddress;

        //Block als belegt makieren
        *rightPos = (unsigned) size;
        rightPos++;

        //Problem sicher stellen das genug Byte übrig sind
        return (void*) rightPos;
    }

	int getSize() {
        return memory.getSize();
    }

    void fillArray(char* array) {/*
        char* ptr1 = (char*) head;//move zeiger
        void* ptr3 = memory.getStart();//vergleich Zeiger zeigt auf naechsten free Block
        int count = 0;

        while (ptr3 != 0) {
            if (ptr1 == ptr3) {
                array[count] = 'M';
                count += sizeof(nextFreeUnit);
                array[count] = 'F';
                count += (((nextFreeUnit*) ptr1) -> freeSpace);

                ptr3 = (((nextFreeUnit*) ptr1) -> nextAddress);
                ptr1 += (((nextFreeUnit*) ptr1) -> freeSpace);
                ptr1 += sizeof(nextFreeUnit);

            } else {
                array[count] = 'M';
                count += sizeof(unsigned);
                array[count] = 'B';
                count += *((unsigned*) ptr1);

                ptr1 += *((unsigned*) ptr1);
                ptr1 += sizeof(unsigned);
            }
        }

        while (ptr1 != sbrk(0)) {
            array[count] = 'M';
            count += sizeof(unsigned);
            array[count] = 'B';
            count += *((unsigned*) ptr1);

            ptr1 += *((unsigned*) ptr1);
            ptr1 += sizeof(unsigned);
        }*/
    }

	bool getList(int i) {
        return 0;
    }

    void merge(void* deletedBlock) {
        //verbinden von mindestens zwei Blöcken
    }

    void free(void* address) {/*
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
        }*/
    }

private:
    nextFreeUnit* head;

};

#endif