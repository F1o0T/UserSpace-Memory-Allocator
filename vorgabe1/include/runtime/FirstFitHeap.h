#ifndef FirstFitHeap_h
#define FirstFitHeap_h

#include <iostream>
#include "runtime/Heap.h"
#include "system/BSSMemory.h"
#include <vector>

using namespace std;

#define minByte sizeof(freeBlock)
#define sizeUnsi sizeof(unsigned)

struct freeBlock {
    unsigned freeSpace;//ist immer der frei Platz - 4 byte für die die groesse des Blockes
    freeBlock* nextAddress;
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
        freeBlock* first = (freeBlock*) memory.getStart();

        this -> head = first;
        first -> freeSpace = length - sizeUnsi;
        first -> nextAddress = 0;

        //erste direkte Liste angelegt an den Anfang von Memory mit ein free-Block mit Laenge length
    }

    void* alloc(size_t size) {
        //Error abfangen
        if (size == 0) {
            cerr << "Error: Bitte keine 0 eingeben!" << endl;
            return nullptr;

        } else if (size < 12) {
            cerr << "Error: Bitte mindestens 12 Bytes anfordern!" << endl;
            return nullptr;
        }
        
        freeBlock* lastPos = 0;//Pointer der auf den FreeBlock vor dem richtigen Block zeigt
        freeBlock* curPos = this -> head;//Pointer der auf einen passenden Block zeigt
        char* ar_ptr = 0;//Pointer der durch den gleich belegten Block durchgeht und dahinter ein neuen freeBlock anlegt

        while ((size_t) (curPos -> freeSpace) < size) {

            if (curPos -> nextAddress == 0) {
                int i = memory.getSize();
                memory.expand(size);//Problem was wenn expand fehlschlägt???
                
                curPos -> freeSpace += (memory.getSize() - i);
                cout << "LastBlock after expand new freespace : " << curPos -> freeSpace << endl;
                break;
            }

            lastPos = curPos;
            curPos = curPos -> nextAddress;
        }

        if ((curPos -> freeSpace) - size < minByte) {
            cout << "weniger als 16 Byte uebrig" << endl;
            if (curPos -> nextAddress == 0) {//letzter Block
                memory.expand(1);//mindest Wert expand
                curPos -> freeSpace += 1024;

            } else {//Mitten im Heap
                size = (curPos -> freeSpace);
                cout << "kein neuer FreeBlock" << endl;

                if (curPos == head) {
                    head = curPos -> nextAddress;

                } else {
                    lastPos -> nextAddress = curPos -> nextAddress;
                }
            }
        }

        //geht an Position fuer den neuen FreeBlock
        ar_ptr = (char*) curPos;
        ar_ptr += sizeUnsi;
        ar_ptr += size;

        //neuen FreeBlock anlegen Falls noch genügend Platz ist
        if ((size_t) (curPos -> freeSpace) > size) {
            cout << "es wird ein neuer FreeBlock angelegt" << endl;

            //wenn der Head genug Platz hatte muss ein neuer Head bestimmt werden
            if (this -> head == curPos) { 
                this -> head = ((freeBlock*) ar_ptr);
                cout << "hier1 " << endl;

            } else { //else muss der FreeBlock davor auf einen neuen Blockzeigen
                lastPos -> nextAddress = ((freeBlock*) ar_ptr);
                cout << "hier2" << endl;
            }

            ((freeBlock*) ar_ptr) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size - sizeUnsi;
            ((freeBlock*) ar_ptr) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
            cout << "hier3 " << ((freeBlock*) ar_ptr) -> freeSpace << endl;
        }

        //die Groesse des Blockes festhalten
        *((unsigned*) curPos) = (unsigned) size;

        //Start des nutzbaren Blockes zurückgeben
        return (void*) (((unsigned*) curPos) + 1);
    }

	int getSize() {
        return memory.getSize();
    }

    void fillList(list<int>* list) {
        char* ptr1 = (char*) memory.getStart();//move zeiger
        void* ptr2 = head;//vergleich Zeiger zeigt auf naechsten free Block

        while (ptr2 != 0) {
            if (ptr1 == ptr2) {
                //Liste beschreiben
                list -> push_back(-1);// -1 = FreeBlockManagment
                list -> push_back(((freeBlock*) ptr1) -> freeSpace);

                //ptr weiterschieben
                ptr2 = (((freeBlock*) ptr1) -> nextAddress);
                ptr1 += (((freeBlock*) ptr1) -> freeSpace);
                ptr1 += sizeUnsi;

            } else {
                list -> push_back(-2);// -2 = BlockedBlockManagment
                list -> push_back(*((unsigned*) ptr1));

                //ptr weiterschieben
                ptr1 += *((unsigned*) ptr1);
                ptr1 += sizeUnsi;
            }
        }
    }

    void merge(freeBlock* block1, freeBlock* block2) {
        if(((char*)block1 + (block1 ->freeSpace)) == ((char*)block2)){
            block1->freeSpace = block1->freeSpace + block2->freeSpace;
            block1->nextAddress = block2->nextAddress;

        }
    }

    void addBlockInList(freeBlock* block){
        freeBlock* pred = head;
        while(pred < block && pred != NULL){
            pred = pred -> nextAddress;
        }
        block->nextAddress = pred;
    }



    void free(void* address) {
        unsigned* blockStart = (unsigned*) address - 1;
        unsigned int blockSize = *((unsigned*) blockStart);        //ob das hier richtig ist, bin ich noch gespannt, es soll den Größenwert auslesen, der an dieser Stelle gespeichert sein sollte
        freeBlock* block = (freeBlock*) blockStart;
        block->freeSpace = blockSize;
        addBlockInList(block); 

    }

private:
    freeBlock* head;

};

#endif