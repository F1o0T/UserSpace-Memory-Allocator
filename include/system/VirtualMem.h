#ifndef VirtualMem_h
#define VirtualMem_h
#include "system/Memory.h"
#include "system/AddressMapping.h"
#include "misc/SwapFile.h"
#include "misc/AccessQueue.h"
#include <iostream>
#include <signal.h>
#include <list>
#include <sys/stat.h>
#include <fcntl.h>
#include <iterator>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>

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

    unsigned pageoutPointer = 0;
    unsigned numberOfPF = 0 ;
    
    int fd = 0; 
    AddressMapping mappingUnit;
    size_t pinnedPages = 0;
    SwapFile swapFile;
    
public:
    unsigned pagesinRAM = 0;
    Queue accessQueue;
    bool LRU_running = false;
    /////////////////////////////////////////////////
    // Signal handeler, constructor and deconstructor.
    // static void signalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
    // VirtualMem(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
    VirtualMem();
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
    void protNoneAll();
};

extern VirtualMem vMem;
void runLRUTimer();

#endif