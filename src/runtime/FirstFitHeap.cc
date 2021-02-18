#include "runtime/FirstFitHeap.h"

VirtualMem* FirstFitHeap::memory = new VirtualMem();
freeBlock* FirstFitHeap::head = (freeBlock*) memory -> getStart();

void FirstFitHeap::signalHandler(int sigNUmber, siginfo_t *info, void *ucontext)
{
	if(info->si_code == SEGV_ACCERR)
    {   
        //cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        memory -> fixPermissions(info->si_addr);
        //cout << "|>>> Permissions were fixed!" << endl;
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }
}

FirstFitHeap::FirstFitHeap() {}

void FirstFitHeap::initHeap() {
    ///////////////////////////////////////////////
	// SignalHandling
    struct sigaction SigAction;
	SigAction.sa_sigaction = signalHandler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
	///////////////////////////////////////////////
    unsigned length = (unsigned) memory -> getSize();

    head -> freeSpace = length - sizeUnsi;
    head -> nextAddress = 0;
    //create first direct list at the start of the memory
    //With only one free block with the length of memory - unsigned
    memory -> setInterval();
}

void* FirstFitHeap::malloc(size_t size) {
    cout << "## Custom Malloc "  << size << endl;
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

    //Return the start of the usable block
    return (void*) (((unsigned*) curPos) + 1);
}

int FirstFitHeap::getSize() {
    return memory -> getSize();
}

void FirstFitHeap::fillList(list<int>* list) {
    char* ptr1 = (char*) memory -> getStart();//move pointer
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

    if(address < memory -> getStart()){
        cerr << "Error: Address to free is smaller than start of the heap" << endl;
        return;
    }
    if(address > (((char*) memory -> getStart()) + memory -> getSize())){
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
    char* ptr1 = (char*) memory -> getStart();//move zeiger
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
    
void* FirstFitHeap::realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    } else if (size == 0 && ptr != NULL) {
        free(ptr);
        return NULL;
    } else if (correctAddress((void*) (( (unsigned*) ptr) - 1)) == false) {
        return NULL;
    }

    void* returnPtr = malloc(size);
    caddr_t oldPosPtr = (caddr_t) ptr;
    caddr_t newPosPtr = (caddr_t) returnPtr;
    //if it is to big then return NULL
    if (newPosPtr == NULL) {
        return NULL;
    }

    for (unsigned i = 0; i < size; i++) {
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
    
    for (unsigned i = 0; i < nmemb*size; i++) {
        *(ptr) = 0;
        ptr++;
    }

    return returnPtr ;
}


void* FirstFitHeap::operator new(size_t size) {
    cout << "new" << endl; 
    return malloc(size);
}

void* FirstFitHeap::operator new[](size_t size) {
    cout << "new []" << endl; 
    return malloc(size);
}

void FirstFitHeap::operator delete(void* ptr) {
    free(ptr);
}

void FirstFitHeap::operator delete[](void* ptr) {
    free(ptr);

}

void FirstFitHeap::destroyTimer() {
    memory -> stopTimer();
}