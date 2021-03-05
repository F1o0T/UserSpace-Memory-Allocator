#include "runtime/FirstFitHeap.h"
#include <unistd.h>
//FirstFitHeap heap;

bool initialized = 0;
VirtualMem e_vMem;
freeBlock* someHead;




FirstFitHeap::FirstFitHeap(){
}



void FirstFitHeap::signalHandler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    //wait on ProtNoneAll
    while(true){
        if (e_vMem.protNoneAllFlag == false) {
            e_vMem.stopTimer();
            break;
        }
        cout << "ok lets wait for protNone1" << endl;
    }

	if (info->si_code == SEGV_ACCERR)
    {   
        //cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        e_vMem.fixPermissions(info->si_addr);
        //cout << "|>>> Permissions were fixed!" << endl;
    }
    else if (info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped @ address = " << info->si_addr << endl; 
        exit(1);
    }
    
	e_vMem.startTimer();

    //wait on ProtNoneAll
    while(true){
        if (e_vMem.protNoneAllFlag == false) {
            break;
        }
        cout << "ok lets wait for protNone2" << endl;
    }
}
/*
FirstFitHeap::FirstFitHeap() {
    e_vMem.setInterval();
}
*/

FirstFitHeap::~FirstFitHeap(){
    cout << "destructor ffheap" << endl;
    while(true){
        if (e_vMem.protNoneAllFlag == false) {
            e_vMem.stopTimer();
            break;
        }
    }
    destroyTimer();
}

void FirstFitHeap::initHeap(){

    initialized = 1;
    someHead = NULL;

    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
}



void* FirstFitHeap::malloc(size_t size) {
    if(!initialized){
        initHeap();
        return sbrk(size); 
    }

    if(someHead == NULL){
        someHead = (freeBlock*) e_vMem.getStart();
        (*(someHead)).freeSpace = (unsigned) e_vMem.getSize() - sizeUnsi;
    }


    //user cannot allocate 0 byte
    if (size == 0) {
        cerr << "Error: Please dont use a 0!" << endl;
        return nullptr;
    }

    freeBlock* lastPos = 0;//Pointer to the free block before the right block
    freeBlock* curPos = someHead;//Pointer that points to a matching block
    char* startFreeMemRest = 0;//Pointer thats create a new free block behind the now allocated memory
    size = setRightSize(size);


    //find free memory block with the needed size
    while ((size_t) (curPos -> freeSpace) < size) {
        if (curPos -> nextAddress == 0) {                
            cerr << "Error: There is not enough memory available." << endl;
            return nullptr;
        }
        lastPos = curPos;
        curPos = curPos -> nextAddress;
    }

    // if rest memory block after allocating would be smaller than the size of a freeBlock (meta data), this is also given to the user
    if ((curPos -> freeSpace) - size < sizeof(freeBlock)) {
        size = (curPos -> freeSpace);

        if (curPos == someHead) {
            someHead = curPos -> nextAddress;

        } else {
            lastPos -> nextAddress = curPos -> nextAddress;
        }
    } else {
        //goes to the Position for the next free block
        startFreeMemRest = (char*) curPos;
        startFreeMemRest += sizeUnsi;
        startFreeMemRest += size;

        //if the matching block were the someHead then create a new someHead
        if (someHead == curPos) { 
            someHead = ((freeBlock*) startFreeMemRest);

        } else { //else the free block before the matching block need to point on him
            lastPos -> nextAddress = ((freeBlock*) startFreeMemRest);
        }

        ((freeBlock*) startFreeMemRest) -> freeSpace = (((freeBlock*) curPos) -> freeSpace) - size;
        ((freeBlock*) startFreeMemRest) -> nextAddress = ((freeBlock*) curPos) -> nextAddress;
    }

    //record the size of the block
    *((unsigned*) curPos) = (unsigned) size;
 
    // cout << "The returned address by malloc will be : " << (void*) (((unsigned*) curPos) + 1) << endl;
    //Return the start of the usable block
    return (void*) (((unsigned*) curPos) + 1);
}

/*
takes the size of a memory block the user wants to allocate and returns the size
the block will actually have because of the meta data of the FirstFitHeap

*/
size_t FirstFitHeap::setRightSize(size_t size) {

    size_t rightSize = size + 4; //increase size, because each memory block given to the user stores an unsigned of meta data (its blocksize)

    if (size < sizeof(freeBlock)) {//if size is smaller than meata data freeFblock, size is set to size of meta data free block
        rightSize = sizeof(freeBlock);
    }

    return rightSize;
}






void FirstFitHeap::fillList(list<int>* list) {
    char* ptr1 = (char*) e_vMem.getStart();//move pointer
    void* ptr2 = someHead;//comparison pointer points on the next free block

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
    freeBlock* succ = someHead;
    while(succ < block && succ != NULL){
        pred = succ;
        succ = succ -> nextAddress;
    }
    block->nextAddress = succ;

    if(someHead > block){
        someHead = block;
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

    cout << "give the address of free = " << address << endl;

    if(address < e_vMem.getStart()){
        cerr << "Error: Address to free is smaller than start of the heap" << endl;
        return;
    }
    if(address > (((char*) e_vMem.getStart()) + e_vMem.getSize())){
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
    char* ptr1 = (char*) e_vMem.getStart();//move zeiger
    void* ptr2 = someHead;//comparison pointer points on the next free block

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

void FirstFitHeap::destroyTimer() {
    e_vMem.stopTimer();
    e_vMem.deleteInterval();
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
    if(!initialized){

    }
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

void* FirstFitHeap::calloc(size_t nmemb, size_t size) {
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

void* FirstFitHeap::operator new(size_t size) {
    cout << "new FirstFitHeap object" << endl; 
    return malloc(size);
}

void* FirstFitHeap::operator new[](size_t size) {
    return malloc(size);
}

void FirstFitHeap::operator delete(void* ptr) {
    cout << "delete FirstFitHeap object" << endl; 
    free(ptr);
}

void FirstFitHeap::operator delete[](void* ptr) {
    free(ptr);
}