#include <iostream>
#include "system/VirtualMem.h"

using namespace std; 

#define minByte sizeof(freeBlock)
#define sizeUnsi sizeof(unsigned)

struct freeBlock {
    unsigned freeSpace;//ist immer der frei Platz - 4 byte für die die groesse des Blockes
    freeBlock* nextAddress;
};

VirtualMem vMem;
bool initalized = false;
freeBlock* head;

void destroyTimer() {
    vMem.deleteInterval();
}

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void signalHandler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    //wait on ProtNoneAll
    while(true){
        if (vMem.protNoneAllFlag == false) {
            vMem.stopTimer();
            break;
        }
        cout << "ok lets wait for protNone1" << endl;
    }

	if (info->si_code == SEGV_ACCERR)
    {   
        //cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        vMem.fixPermissions(info->si_addr);
        //cout << "|>>> Permissions were fixed!" << endl;
    }
    else if (info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped @ address = " << info->si_addr << endl; 
        exit(1);
    }
    
	vMem.startTimer();

    //wait on ProtNoneAll
    while(true){
        if (vMem.protNoneAllFlag == false) {
            break;
        }
        cout << "ok lets wait for protNone2" << endl;
    }
}

void initHeap() {
    ///////////////////////////////////////////////
	// SignalHandling
    struct sigaction SigAction;
	SigAction.sa_sigaction = signalHandler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
	///////////////////////////////////////////////
    unsigned length = (unsigned) vMem.getSize();

    vMem.initFirstPageForHeap();
    head  = (freeBlock*) vMem.getStart();
    head -> freeSpace = length - sizeUnsi;
    head -> nextAddress = 0;
    //create first direct list at the start of the memory
    //With only one free block with the length of memory - unsigned
    //vMem.setInterval();
}

void* malloc(size_t size) {

    if(initalized == false) {
        initalized = true;
        vMem.initMem();
        initHeap();
        vMem.accessStack.insertPageAtTop(vMem.getStart());
        vMem.addPageEntry2PT((unsigned*) vMem.getStart());
        vMem.writePageActivate(vMem.getStart());
    }
    //cout << "## Custom Malloc "  << size << endl;
    if (size == 0) {
        cerr << "Error: Please dont use a 0!" << endl;
        return nullptr;

    } else if (size < 12) {
        size = 12;
    }

    freeBlock* lastPos = 0;//Pointer to the free block before the right block
    freeBlock* curPos = head;//Pointer that points to a matching block
    char* ar_ptr = 0;//Pointer thats create a new free block behind the matching block

    while ((size_t) (curPos -> freeSpace) < size) {

        if (curPos -> nextAddress == 0) {                
            cerr << "Error: There is not enough memory available." << endl;
            return nullptr;
        }

        lastPos = curPos;
        curPos = curPos -> nextAddress;
    }

    // if the next freeBlock didnt have enough memory
    if ((curPos -> freeSpace) - size < minByte) {
        size = (curPos -> freeSpace);

        if (curPos == head) {
            head = curPos -> nextAddress;

        } else {
            lastPos -> nextAddress = curPos -> nextAddress;
        }
    } else {
        //goes to the Position for the next free block
        ar_ptr = (char*) curPos;
        ar_ptr += sizeUnsi;
        ar_ptr += size;

        //if the matching block were the head then create a new head
        if (head == curPos) { 
            head = ((freeBlock*) ar_ptr);

        } else { //else the free block before the matching block need to point on him
            lastPos -> nextAddress = ((freeBlock*) ar_ptr);
        }

        ((freeBlock*) ar_ptr) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size - sizeUnsi;
        ((freeBlock*) ar_ptr) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
    }

    //record the size of the block
    *((unsigned*) curPos) = (unsigned) size;
 
    // cout << "The returned address by malloc will be : " << (void*) (((unsigned*) curPos) + 1) << endl;
    //Return the start of the usable block
    return (void*) (((unsigned*) curPos) + 1);
}

void merge(freeBlock* block1, freeBlock* block2) {
        block1->freeSpace = block1->freeSpace + (block2->freeSpace + sizeUnsi);
        block1->nextAddress = block2->nextAddress;
}

void addBlockInList(freeBlock* block){
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

//checks whether the address to free is a correct start of a block
bool correctAddress(void* address){
    char* ptr1 = (char*) vMem.getStart();//move zeiger
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

void free(void* ptr) {

    if(ptr < vMem.getStart()){
        cerr << "Error: Address to free is smaller than start of the heap" << endl;
        return;
    }
    if(ptr > (((char*) vMem.getStart()) + vMem.getSize())){
        cerr << "Error: Address to free is bigger than end of the heap" << endl;
        return;
    }
    list<void*> blockList;


    
    if(!correctAddress((void*) (((unsigned*) ptr) - 1))){
        cerr << "Error: Address in Heap can't be freed" << endl;
        return;
    }


    unsigned* blockStart = ((unsigned*) ptr) - 1;
    unsigned int blockSize = *((unsigned*) blockStart);
    freeBlock* block = (freeBlock*) blockStart;
    block->freeSpace = blockSize;
    addBlockInList(block); 
}

void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    } else if (size == 0) {
        free(ptr);
        return NULL;
    } else if (correctAddress((void*) (( (unsigned*) ptr) - 1)) == false) {
        return NULL;
    }

    void* returnPtr = malloc(size);
    //if it is to big then return NULL
    if (returnPtr == NULL) {
        return NULL;
    }
    caddr_t oldPosPtr = (caddr_t) ptr;
    caddr_t newPosPtr = (caddr_t) returnPtr;

    for (size_t i = 0; i < size; i++) {
        *(newPosPtr) = *(oldPosPtr);
        newPosPtr++;
        oldPosPtr++;
    }

    free(ptr);
    return returnPtr;
}

void* calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) {
        return NULL;
    }

    void* returnPtr = malloc(size);
    caddr_t ptr = (caddr_t) returnPtr;
    //if it is to big then return NULL
    if (ptr == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < nmemb*size; i++) {
        *(ptr) = 0;
        ptr++;
    }

    return returnPtr ;
}