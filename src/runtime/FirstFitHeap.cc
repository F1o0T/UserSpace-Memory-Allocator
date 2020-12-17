#include "runtime/FirstFitHeap.h"

FirstFitHeap::FirstFitHeap(Memory& memory) : Heap(memory) {}

void FirstFitHeap::initHeap() {
    freeBlock* first = (freeBlock*) memory.getStart();
    unsigned length = (unsigned) memory.getSize();

    this -> head = first;
    first -> freeSpace = length - sizeUnsi;
    first -> nextAddress = 0;

    //create first direct list at the start of the memory
    //With only one free block with the length of memory - unsigned
}

void* FirstFitHeap::alloc(size_t size) {
    //catch Errors
    if (size == 0) {
        cerr << "Error: Please dont use a 0!" << endl;
        return nullptr;

    } else if (size < 12) {
        size = 12;
    }
    
    freeBlock* lastPos = 0;//Pointer to the free block before the right block
    freeBlock* curPos = this -> head;//Pointer that points to a matching block
    char* ar_ptr = 0;//Pointer thats create a new free block behind the matching block

    while ((size_t) (curPos -> freeSpace) < size) {

        if (curPos -> nextAddress == 0) {
            int i = memory.getSize();
            void* ptr = memory.expand(size);
            
            if (ptr == 0 || ptr == (void*) -1) {
                cerr << "Error: There is not enough memory available." << endl;
                return nullptr;
            }
            
            curPos -> freeSpace += (memory.getSize() - i);
            break;
        }

        lastPos = curPos;
        curPos = curPos -> nextAddress;
    }

    if ((curPos -> freeSpace) - size < minByte) {
        if (curPos -> nextAddress == 0) {//last block in heap
            int i = memory.getSize();
            void* ptr = memory.expand(1);//minimum to expand

            if (ptr == 0 || ptr == (void*) -1) {
                cerr << "Error: Es kann nicht genug Speicher zur verfügung gestellt werden." << endl;
                return nullptr;
            }
            curPos -> freeSpace += (memory.getSize() - i);

        } else {//somewhere in the middle of the heap
            size = (curPos -> freeSpace);

            if (curPos == head) {
                head = curPos -> nextAddress;

            } else {
                lastPos -> nextAddress = curPos -> nextAddress;
            }
        }
    }

    //goes to the Position for the next free block
    ar_ptr = (char*) curPos;
    ar_ptr += sizeUnsi;
    ar_ptr += size;

    //if enough space left then create a new free block
    if ((size_t) (curPos -> freeSpace) > size) {

        //if the matching block were the head then create a new head
        if (this -> head == curPos) { 
            this -> head = ((freeBlock*) ar_ptr);

        } else { //else the free block before the matching block need to point on him
            lastPos -> nextAddress = ((freeBlock*) ar_ptr);
        }

        ((freeBlock*) ar_ptr) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size - sizeUnsi;
        ((freeBlock*) ar_ptr) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
    }

    //record the size of the block
    *((unsigned*) curPos) = (unsigned) size;

    //Return the start of the usable block
    return (void*) (((unsigned*) curPos) + 1);
}

int FirstFitHeap::getSize() {
    return memory.getSize();
}

void FirstFitHeap::fillList(list<int>* list) {
    char* ptr1 = (char*) memory.getStart();//move pointer
    void* ptr2 = head;//comparison pointer points on the next free block

    while (ptr2 != 0) {
        if (ptr1 == ptr2) {
            //fill list
            list -> push_back(-1);// -1 = FreeBlockManagment
            list -> push_back(((freeBlock*) ptr1) -> freeSpace);

            //move on ptr
            ptr2 = (((freeBlock*) ptr1) -> nextAddress);
            ptr1 += (((freeBlock*) ptr1) -> freeSpace);
            ptr1 += sizeUnsi;

        } else {
            list -> push_back(-2);// -2 = BlockedBlockManagment
            list -> push_back(*((unsigned*) ptr1));

            //move on ptr
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


//checks whether the address to free is a correct start of a block
bool FirstFitHeap::correctAddress(void* address){
    char* ptr1 = (char*) memory.getStart();//move zeiger
    void* ptr2 = head;//comparison pointer points on the next free block

    while (ptr2 != 0) {
        if (ptr1 == ptr2) {
            //move on ptr
            ptr2 = (((freeBlock*) ptr1) -> nextAddress);
            ptr1 += (((freeBlock*) ptr1) -> freeSpace);
            ptr1 += sizeUnsi;

        } else {
            //move on ptr
            if(address == ptr1){
                return true;
            }

            ptr1 += *((unsigned*) ptr1);
            ptr1 += sizeUnsi;
        }
    }

    return false;

}
