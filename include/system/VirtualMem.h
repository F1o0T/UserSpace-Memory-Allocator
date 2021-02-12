#ifndef VirtualMem_h
#define VirtualMem_h

#include "system/Memory.h"
#include "misc/RandomAccessFile.h"
#include "system/AddressMapping.h"

#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <list>

#include <sys/stat.h>
#include <fcntl.h>

#include <iterator>
#include <map>

using namespace std;

class SwapFile : public RandomAccessFile
{

public:
    int fd;

    SwapFile()
    {
        fd = open("SwapFile", O_RDWR | O_CREAT | O_TRUNC);
        if (fd == -1)
        {
            cerr << "Error Openining the swap file";
            exit(1);
        }
    }

    ~SwapFile()
    {
        close(fd);
        if (unlink("SwapFile") == -1)
        {
            cerr << "Error unlink didn't delete the swapfile" << endl;
        }
    }

    /**
            This function reads a specified amount of bytes from file to memory.
            Beginning at the offset in the file.
 
            @param addr is the address in memory, where the data is copied to
            @param offset is the start position in the file, where the data is copied from
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
    virtual ssize_t swapFileRead(void *addr, off_t offset, size_t bytes)
    {
        lseek(fd, offset, SEEK_SET);
        read(fd, addr, bytes);
        return bytes;
    }

    /**
            This function writes a specified amount of bytes from memory to file.
            Beginning at the offset in the file.

            @param addr is the address in memory, where the data are copied from
            @param offset is the start position in the file, where the data are copied to
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
    virtual ssize_t swapFileWrite(void *addr, off_t offset, size_t bytes)
    {
        lseek(fd, offset, SEEK_SET);
        write(fd, addr, bytes);
        return 0;
    }

    /**
            This function reserves a specified amount of bytes.
            Beginning at the offset in the file.

            @param offset is the position in file, where reservation starts
            @param bytes is the amount of bytes, which should be reserved
            @return value is the amount of bytes which could be reserved
        */
    virtual ssize_t swapFilereserve(off_t offset, size_t bytes)
    {
        return 0;
    }
};

enum permission_change : int
{
    NONTOREAD_NOTFULL,
    NONTOREAD_FULL,
    READTOWRITE
};

struct stackPageNode
{
    void *pageAddress;
    stackPageNode *prev, *next;
};
class Stack
{
    stackPageNode *top;
    stackPageNode *bottom;
    int stackSize;
public:
    Stack()
    {
        top = bottom = NULL;
        stackSize = 0;
    }
    // Operations on Deque
    void insertPageAtBottom(void *pageAddr)
    {
        stackPageNode *newStackPageNode = new stackPageNode; 
        newStackPageNode->pageAddress = pageAddr; 
        newStackPageNode->prev = newStackPageNode->next = NULL;

        if (bottom == NULL)
            top = bottom = newStackPageNode;
        else
        {
            newStackPageNode->prev = bottom;
            bottom->next = newStackPageNode;
            bottom = newStackPageNode;
        }
        stackSize++; 
    }
    void insertPageAtTop(void *pageAddr)
    {
        stackPageNode *newStackPageNode = new stackPageNode; 
        newStackPageNode->pageAddress = pageAddr; 
        newStackPageNode->prev = newStackPageNode->next = NULL;

        if (top == NULL)
            bottom = top = newStackPageNode;
        else
        {
            newStackPageNode->next = top;
            top->prev = newStackPageNode;
            top = newStackPageNode;
        }
        stackSize++;
    }

    void deletePageAtBottom()
    {
        stackPageNode *temp = bottom;
        bottom = bottom->prev;

        // If only one element was present
        if (bottom == NULL)
            top = NULL;
        else
            bottom->next = NULL;
        delete(temp);

        // Decrements count of elements by 1
        stackSize--;
    }

    void deletePageAtTop()
    {
        stackPageNode *temp = top;
        top = top->next;

        // If only one element was present
        if (top == NULL)
            bottom = NULL;
        else
            top->prev = NULL;
        free(temp);

        // Decrements count of elements by 1
        stackSize--;
    }

    void deletePageIfExists(void *page)
    {
        // b ../src/system/VirtualMem.cc:76
        // If deque is empty then
        // 'Underflow' condition
        if (isEmpty())
            return; 
        if (top->pageAddress == page)
        {
            deletePageAtTop();
        }  
        else if (bottom->pageAddress == page)
        {
            deletePageAtBottom();
        }
        else
        {
            stackPageNode *current = top; 
            while(current != NULL)
            {
                if(current->pageAddress == page)
                {
                    current->prev->next = current->next; 
                    delete current; 
                    stackSize--;
                    break; 
                }
                current = current->next;
            }
        } 
    }
    void* getPageAtbottom()
    {
        // If deque is empty, then returns
        // garbage value
        if (isEmpty())
            return (void*)-1;
        return bottom->pageAddress;
    }
    void* getPageAtTop()
    {
        // If deque is empty, then returns
        // garbage value
        if (isEmpty())
            return (void*)-1;
        return top->pageAddress;
    }
    int size()
    {
        return stackSize;
    }
    bool isEmpty()
    {
        return (bottom == NULL);
    }
    void display()
    {
        stackPageNode *current = top; 
        while(current != NULL)
        {
            cout << current->pageAddress << endl;
            current = current->next;
        }
    }
    void destroy()
    {
        top = NULL;
        while (bottom != NULL)
        {
            stackPageNode *temp = bottom;
            bottom = bottom->next;
            free(temp);
        }
        stackSize = 0;
    }
};
class VirtualMem
{
private:
    unsigned *virtualMemStartAddress = NULL;
    unsigned nextFreeFrameIndex = 0;
    unsigned pagesinRAM = 0;
    unsigned pageoutPointer = 0;
    unsigned numberOfPF = 0;
    int fd = 0;
    AddressMapping mappingUnit;
    size_t pinnedPages = 0;
    SwapFile swapFile;
    
public:
    Stack accessStack; 
    /////////////////////////////////////////////////
    // Signal handeler, constructor and deconstructor.
    // static void signalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
    // VirtualMem(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
    void initializeVirtualMem(unsigned numberOfPF);
    ~VirtualMem();
    /////////////////////////////////////////////////
    // Basic Methods
    void *getStart();
    size_t getSize();
    void *expand(size_t size);
    /////////////////////////////////////////////////
    // Advanced Methods
    void fixPermissions(void *);
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
    /////////////////////////////////////////////////

};

#endif