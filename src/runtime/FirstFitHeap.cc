#include "runtime/FirstFitHeap.h"
#include <unistd.h>

bool initialized = 0;


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



FirstFitHeap::FirstFitHeap() {
    initialized = 1;
    this->head = (freeBlock*) (vMem.getStart());

    SigAction.sa_sigaction = signalHandler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
    this->head->freeSpace = (unsigned) vMem.getSize();

    vMem.setInterval();
}



FirstFitHeap::~FirstFitHeap(){
    cout << "destructor ffheap" << endl;
    destroyTimer();
}

void FirstFitHeap::initHeap(){
}



void* FirstFitHeap::malloc(size_t size) {
    
    if(!initialized){
        return sbrk(size);
    }
    //user cannot allocate 0 byte
    if (size == 0) {
        cerr << "Error: Please dont use a 0!" << endl;
        return nullptr;
    }
    /////////////start normal method
    freeBlock* lastPos = 0;//Pointer to the free block before the right block
    freeBlock* curPos = this->head;//Pointer that points to a matching block
    freeBlock* startFreeMemRest = 0;//Pointer thats create a new free block behind the now allocated memory


    //find free memory block with the needed size
    while ((size_t) (curPos -> freeSpace) < size + 4 || curPos -> freeSpace < sizeof(freeBlock)) {
        if (curPos -> nextAddress == 0) {                
            cerr << "Error: There is not enough memory available." << endl;
            return nullptr;
        }
        lastPos = curPos;
        curPos = curPos -> nextAddress;
    }

    //set the correct size of the memory, given to the user
    size = setRightSize(curPos, size);

    
    //if user gets whole block of memory or not do different pointer stuff
    if(curPos -> freeSpace == size){
        if (curPos == this->head) {
            this->head = curPos -> nextAddress;
        }else{
            lastPos -> nextAddress = curPos -> nextAddress;
        }
    }else{
        startFreeMemRest = (freeBlock*) ((char*)curPos + size);
        if (this->head == curPos) { 
            this->head = ((freeBlock*) startFreeMemRest);
        }else{
            lastPos -> nextAddress = ((freeBlock*) startFreeMemRest);
        }

        ((freeBlock*) startFreeMemRest) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size;
        ((freeBlock*) startFreeMemRest) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
    }

    //record the size of the block
    *((unsigned*) curPos) = (unsigned) size;
 
    //cout << "The returned address by malloc will be : " << (void*) (((unsigned*) curPos) + 1) << endl;
    //Return the start of the usable block
    return (void*) (((unsigned*) curPos) + 1);

}

/*
takes the size of a memory block the user wants to allocate and returns the size
the block will actually have because of the meta data of the FirstFitHeap
@param memblock with enough size

*/
size_t FirstFitHeap::setRightSize(freeBlock* memBlock, size_t size) {

    size_t rightSize = size + 4; //increase size, because each memory block given to the user stores an unsigned of meta data (its blocksize)

    if (size < sizeof(freeBlock)) {//if size is smaller than meata data freeFblock, size is set to size of meta data free block
        rightSize = sizeof(freeBlock);
    }

    //if rest of freeBlock would be too small, give the user the whole free block
    if (memBlock->freeSpace - rightSize < sizeof(freeBlock)){
        rightSize = (size_t) memBlock -> freeSpace;
    }

    return rightSize;
}






void FirstFitHeap::fillList(list<int>* list) {
    char* ptr1 = (char*) vMem.getStart();//move pointer
    void* ptr2 = this->head;//comparison pointer points on the next free block

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
        block1->freeSpace = block1->freeSpace + (block2->freeSpace);
        block1->nextAddress = block2->nextAddress;
}

void FirstFitHeap::addBlockInList(freeBlock* block){
    freeBlock* pred = NULL;
    freeBlock* succ = this->head;
    while(succ < block && succ != NULL){
        pred = succ;
        succ = succ -> nextAddress;
    }
    block->nextAddress = succ;

    if(this->head > block){
        this->head = block;
    }
    

    if((((char*)block) + block->freeSpace) == ((char*)succ)){
        merge(block, succ);
    }

    if(pred != NULL){
        pred->nextAddress = block;
        if((((char*)pred) + pred->freeSpace) == ((char*)block)){
            merge(pred, block);
        }
    }
    
}

void FirstFitHeap::free(void* address) {

    cout << "give the address of free = " << address << endl;

    if(address < vMem.getStart()){
        cerr << "Error: Address to free is smaller than start of the heap" << endl;
        return;
    }
    if(address > (((char*) vMem.getStart()) + vMem.getSize())){
        cerr << "Error: Address to free is bigger than end of the heap" << endl;
        return;
    }
    unsigned* blockStart = ((unsigned*) address) - 1;

    
    if(!correctAddress((void*) blockStart)){
        cerr << "Error: Address in Heap can't be freed" << endl;
        return;
    }
 
    unsigned int blockSize = *((unsigned*) blockStart);
    freeBlock* block = (freeBlock*) blockStart;
    block->freeSpace = blockSize;
    addBlockInList(block); 

}

//checks whether the address to free is a correct start of a block
bool FirstFitHeap::correctAddress(void* address){
    char* ptr1 = (char*) vMem.getStart();//move zeiger
    void* ptr2 = this->head;//comparison pointer points on the next free block

    while (ptr2 != 0) {
        if (ptr1 == ptr2) {
            //move on ptr
            ptr2 = (((freeBlock*) ptr1) -> nextAddress);
            ptr1 += (((freeBlock*) ptr1) -> freeSpace);
            //ptr1 += sizeUnsi;

        } else {
            //move on ptr
            if(address == ptr1){
                return true;
            }

            ptr1 += *((unsigned*) ptr1);
            //ptr1 += sizeUnsi;
        }
    }

    return false;

}

void FirstFitHeap::destroyTimer() {
    while(true){
        if (vMem.protNoneAllFlag == false) {
            vMem.stopTimer();
            break;
        }
    }
    vMem.deleteInterval();
}

void* FirstFitHeap::realloc(void* ptr, size_t size) {
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

void* FirstFitHeap::calloc(size_t numEl, size_t size) {
    if (numEl == 0 || size == 0) {
        return NULL;
    }

    void* returnPtr = malloc(numEl * size);
    caddr_t ptr = (caddr_t) returnPtr;
    //if it is to big then return NULL
    if (ptr == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < numEl; i++) {
        *(ptr) = 0;
        ptr++;
    }

    return returnPtr ;
}

void* FirstFitHeap::operator new(size_t size) {
    return heap.malloc(size);
}

void* FirstFitHeap::operator new[](size_t size) {
    return heap.malloc(size);
}

void FirstFitHeap::operator delete(void* ptr) {
    heap.free(ptr);
}

void FirstFitHeap::operator delete[](void* ptr) {
    heap.free(ptr);
}