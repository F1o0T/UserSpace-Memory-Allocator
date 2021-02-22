#ifndef VirtualMem_h
#define VirtualMem_h
#include "system/Memory.h"
#include "system/AddressMapping.h"
#include "misc/SwapFile.h"
#include "misc/Stack.h"
#include <iostream>
#include <signal.h>
#include <list>
#include <sys/stat.h>
#include <fcntl.h>
#include <iterator>
#include <map>
#include <timer/timercpp.h>
using namespace std;



enum permission_change : int
{
    NONTOREAD_NOTFULL,
    NONTOREAD_FULL,
    READTOWRITE,
    LRU_CASE_READ, 
    LRU_CASE_WRITE
};

class VirtualMem: public Memory
{
private:
    
    ///////////////////////////////////////////
    unsigned *virtualMemStartAddress = NULL;
    unsigned nextFreeFrameIndex = 0;
    unsigned pagesinRAM = 0;
    unsigned pageoutPointer = 0;
    unsigned numberOfPF = 0 ;
    
    int fd = 0; 
    AddressMapping mappingUnit;
    size_t pinnedPages = 0;
    SwapFile swapFile;
    
public:
    Timer protNonetimer = Timer();
    bool protNoneAllFlag = false;
    Stack accessStack; 
    /////////////////////////////////////////////////
    // Signal handeler, constructor and deconstructor.
    // static void signalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
    // VirtualMem(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
    VirtualMem();
    void initMem();
    ~VirtualMem();
    /////////////////////////////////////////////////
    // Basic Methods
    void *getStart();
    size_t getSize();
    void *expand(size_t size);
    /////////////////////////////////////////////////
    // Advanced Methods
    void fixPermissions(void *);
    void* kickPageFromStack();
    void initializePDandFirstPT();
    void addPageEntry2PT(unsigned *pageStartAddress);
    void addPTEntry2PD(unsigned *startAddrPage);
    void *findStartAddress(void *ptr);
    void readPageActivate(void *ptr);
    void writePageActivate(void *ptr);
    void pageOut(void *ptr);
    void pageIn(void *ptr);
    void mapOut(void *pageStartAddress);
    void mapIn(void *pageStartAddress);
    void fillList(list<int> *virtualMem, list<unsigned> *physicalMem);
    void resetQueues();
    void protNoneAll();
    void setInterval();
    void deleteInterval();
    void startTimer();
    void stopTimer();
    void initFirstPageForHeap();

    void* operator new(size_t size);

    void operator delete(void* ptr);
};

#endif