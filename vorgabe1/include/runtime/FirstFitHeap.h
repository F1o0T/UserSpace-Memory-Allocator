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
    bool tail;//true wenn es der Tail von gesamten Heap ist(also auch belegte Bloecke)
    unsigned freeSpace;
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
        first -> tail = true;
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
                cout << "hier1" << endl;
                if (curPos -> tail) {
                    curPos -> freeSpace += (memory.getSize() - i);
                    cout << "hier2 " << curPos -> freeSpace << endl;
                } else {//Hoffe das worked
                    lastPos = curPos;
                    cout << "hier3" << endl;
                    ar_ptr = (char*) curPos;
                    int steps = ar_ptr - ((char*) memory.getStart());
                    steps = memory.getSize() - steps;
                    steps -= i;
                    ar_ptr += steps;

                    curPos = (freeBlock*) ar_ptr;

                    curPos -> tail = true;
                    curPos -> freeSpace = (memory.getSize() - i) - sizeUnsi;
                    curPos -> nextAddress = 0;
                }

                break;
            }

            lastPos = curPos;
            curPos = curPos -> nextAddress;
        }

        if ((curPos -> freeSpace) - size < minByte + 12) {
            cout << "hier4" << endl;
            if ((curPos -> tail) == true) {
                cout << "hier5" << endl;
                memory.expand(1);//mindest Wert expand
                curPos -> freeSpace += 1024;

            } else {
                cout << "hier6" << endl;
                size = (curPos -> freeSpace);
            }
        }

        //geht an Position für den neuen FreeBlock
        ar_ptr = (char*) curPos;
        ar_ptr += sizeUnsi;
        ar_ptr += size;

        //Wenn der Block exakt rein gepasst hat dann keinen neuen FreeBlock
        if ((size_t) (curPos -> freeSpace) == size) {
            lastPos -> nextAddress = curPos -> nextAddress;
            cout << "hier7" << endl;

        } else {//else  neuen FreeBlock anlegen
            cout << "hier8" << endl;

            //wenn der Head genug Platz hatte muss ein neuer Head bestimmt werden
            if (this -> head == curPos) { 
                this -> head = ((freeBlock*) ar_ptr);
                cout << "eerwer " << (((freeBlock*) curPos) -> freeSpace) << endl;
                cout << "hier9" << (((freeBlock*) curPos) -> freeSpace) - size << endl;

            } else { //else muss der FreeBlock davor auf einen neuen Blockzeigen
                lastPos -> nextAddress = ((freeBlock*) ar_ptr);
                cout << "hier10" << endl;
            }

            ((freeBlock*) ar_ptr) -> tail = ((freeBlock*) curPos) -> tail;
            ((freeBlock*) ar_ptr) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size - sizeUnsi;
            ((freeBlock*) ar_ptr) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
        }

        //die Groesse des Blockes festhalten
        *((unsigned*) curPos) = (unsigned) size;

        //Start des nutzbaren Blockes zurückgeben
        return (void*) (((unsigned*) curPos) + 1);
    }

	int getSize() {
        return memory.getSize();
    }

    void fillArray(char* array) {
        char* ptr1 = (char*) memory.getStart();//move zeiger
        void* ptr2 = head;//vergleich Zeiger zeigt auf naechsten free Block
        int count = 0;

        while (ptr2 != 0) {
            if (ptr1 == ptr2) {
                //Array beschreiben
                array[count] = 'M';
                count += sizeUnsi;
                cout << count << endl;
                array[count] = 'F';
                count += (((freeBlock*) ptr1) -> freeSpace);
                cout << count << endl;

                //ptr weiterschieben
                ptr2 = (((freeBlock*) ptr1) -> nextAddress);
                cout << (void*) ptr1 << endl;
                ptr1 += (((freeBlock*) ptr1) -> freeSpace);
                ptr1 += sizeUnsi;
                cout << (void*) ptr1 << endl;

            } else {
                cout << "hier11" << endl;
                //Array beschreiben
                array[count] = 'M';
                count += sizeUnsi;
                array[count] = 'B';
                count += *((unsigned*) ptr1);

                //ptr weiterschieben
                ptr1 += *((unsigned*) ptr1);
                ptr1 += sizeUnsi;
            }
        }

        while (count != getSize()) {
            array[count] = 'M';
            count += sizeUnsi;
            array[count] = 'B';
            count += *((unsigned*) ptr1);

            ptr1 += *((unsigned*) ptr1);
            ptr1 += sizeUnsi;
        }
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
    freeBlock* head;

};

#endif