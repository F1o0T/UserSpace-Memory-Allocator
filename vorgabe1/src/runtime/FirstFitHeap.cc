#include "runtime/FirstFitHeap.h"

FirstFitHeap::FirstFitHeap(Memory& memory) : Heap(memory) {
}

void FirstFitHeap::initHeap(int n = 1) {
    unsigned length = 0;
    freeBlock* first = (freeBlock*) memory.getStart();
    void* ptr = 0;

    if (n <= 0) {
        cerr << "Error: Die Heapgroesse darf nicht kleiner gleich 0 sein. Heap wurde auf Standardgroesse gesetzt." << endl;
        n = 1;
    }

    ptr = memory.expand(n);
    if (ptr == 0) {
        cerr << "Error: Es kann nicht genug Speicher zur verfügung gestellt werden." << endl;
        return;
    }

    length = (unsigned) memory.getSize();

    this -> head = first;
    first -> freeSpace = length - sizeUnsi;
    first -> nextAddress = 0;

    //erste direkte Liste angelegt an den Anfang von Memory mit ein free-Block mit Laenge length
}

void* FirstFitHeap::alloc(size_t size) {
    //Error abfangen
    if (size == 0) {
        cerr << "Error: Bitte keine 0 eingeben!" << endl;
        return nullptr;

    } else if (size < 12) {
        size = 12;
    }
    
    freeBlock* lastPos = 0;//Pointer der auf den FreeBlock vor dem richtigen Block zeigt
    freeBlock* curPos = this -> head;//Pointer der auf einen passenden Block zeigt
    char* ar_ptr = 0;//Pointer der durch den gleich belegten Block durchgeht und dahinter ein neuen freeBlock anlegt

    while ((size_t) (curPos -> freeSpace) < size) {

        if (curPos -> nextAddress == 0) {
            int i = memory.getSize();
            void* ptr = memory.expand(size);
            
            if (ptr == 0 || ptr == (void*) -1) {
                cerr << "Error: Es kann nicht genug Speicher zur verfügung gestellt werden." << endl;
                return nullptr;
            }
            
            curPos -> freeSpace += (memory.getSize() - i);
            break;
        }

        lastPos = curPos;
        curPos = curPos -> nextAddress;
    }

    if ((curPos -> freeSpace) - size < minByte) {
        if (curPos -> nextAddress == 0) {//letzter Block
            void* ptr = memory.expand(1);//mindest Wert expand

            if (ptr == 0 || ptr == (void*) -1) {
                cerr << "Error: Es kann nicht genug Speicher zur verfügung gestellt werden." << endl;
                return nullptr;
            }

            curPos -> freeSpace += 1024;

        } else {//Mitten im Heap
            size = (curPos -> freeSpace);

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

        //wenn der Head genug Platz hatte muss ein neuer Head bestimmt werden
        if (this -> head == curPos) { 
            this -> head = ((freeBlock*) ar_ptr);

        } else { //else muss der FreeBlock davor auf einen neuen Blockzeigen
            lastPos -> nextAddress = ((freeBlock*) ar_ptr);
        }

        ((freeBlock*) ar_ptr) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size - sizeUnsi;
        ((freeBlock*) ar_ptr) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
    }

    //die Groesse des Blockes festhalten
    *((unsigned*) curPos) = (unsigned) size;

    //Start des nutzbaren Blockes zurückgeben
    return (void*) (((unsigned*) curPos) + 1);
}

int FirstFitHeap::getSize() {
    return memory.getSize();
}

void FirstFitHeap::fillList(list<int>* list) {
    char* ptr1 = (char*) memory.getStart();//move zeiger
    void* ptr2 = head;//vergleich Zeiger zeigt auf naechsten FreeBlock

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

void FirstFitHeap::merge(freeBlock* block1, freeBlock* block2) {
        block1->freeSpace = block1->freeSpace + (block2->freeSpace + sizeUnsi);
        block1->nextAddress = block2->nextAddress;
}

void FirstFitHeap::addBlockInList(freeBlock* block){
    freeBlock* pred = NULL;
    freeBlock* succ = head;
    while(succ < block && succ != NULL){
        pred = succ;
        succ = succ -> nextAddress;
    }
    block->nextAddress = succ;

    if(head > block){
        head = block;
    }
    

    if((((char*)block) + block->freeSpace + sizeUnsi) == ((char*)succ)){
        merge(block, succ);
    }

    if(pred != NULL){
        pred->nextAddress = block;
        if((((char*)pred) + pred->freeSpace + sizeUnsi) == ((char*)block)){
            merge(pred, block);
        }
    }
    
}

void FirstFitHeap::free(void* address) {

    if(address < memory.getStart()){
        cerr << "Error: Address to free is smaller than start of the heap" << endl;
        return;
    }
    if(address > (((char*) memory.getStart()) + memory.getSize())){
        cerr << "Error: Address to free is bigger than end of the heap" << endl;
        return;
    }
    list<void*> blockList;


    
    if(!correctAddress((void*) (((unsigned*) address) - 1))){
        cerr << "Error: Address in Heap can't be freed" << endl;
        return;
    }


    unsigned* blockStart = ((unsigned*) address) - 1;
    unsigned int blockSize = *((unsigned*) blockStart);
    freeBlock* block = (freeBlock*) blockStart;
    block->freeSpace = blockSize;
    addBlockInList(block); 

}


//checks if the address to free is a correct beginning of a block
bool FirstFitHeap::correctAddress(void* address){
    char* ptr1 = (char*) memory.getStart();//move zeiger
    void* ptr2 = head;//vergleich Zeiger zeigt auf naechsten FreeBlock

    while (ptr2 != 0) {
        if (ptr1 == ptr2) {
            //ptr weiterschieben
            ptr2 = (((freeBlock*) ptr1) -> nextAddress);
            ptr1 += (((freeBlock*) ptr1) -> freeSpace);
            ptr1 += sizeUnsi;

        } else {
            //ptr weiterschieben
            if(address == ptr1){
                return true;
            }

            ptr1 += *((unsigned*) ptr1);
            ptr1 += sizeUnsi;
        }
    }

    return false;

}
