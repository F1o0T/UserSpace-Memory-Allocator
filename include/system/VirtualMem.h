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

enum permission_change : int
{
    NONTOREAD_NOTFULL,
    NONTOREAD_FULL,
    READTOWRITE
};

struct Node
{
    void *pageAddress;
    Node *prev, *next;
};

// A structure to represent a deque
class Deque
{
    Node *front;
    Node *rear;
    int Size;

public:
    Deque()
    {
        front = rear = NULL;
        Size = 0;
    }
    // Operations on Deque
    void insertPageAtFront(void *pageAddr)
    {
        Node *newNode = new Node; 
        newNode->pageAddress = pageAddr; 
        newNode->prev = newNode->next = NULL;

        if (front == NULL)
            rear = front = newNode;
        else
        {
            newNode->next = front;
            front->prev = newNode;
            front = newNode;
        }
        Size++; 
    }
    void insertPageAtRear(void *pageAddr)
    {
        Node *newNode = new Node; 
        newNode->pageAddress = pageAddr; 
        newNode->prev = newNode->next = NULL;

        if (rear == NULL)
            front = rear = newNode;
        else
        {
            newNode->prev = rear;
            rear->next = newNode;
            rear = newNode;
        }
        Size++;
    }
    void deletePageAtFront()
    {
        Node *temp = front;
        front = front->next;

        // If only one element was present
        if (front == NULL)
            rear = NULL;
        else
            front->prev = NULL;
        free(temp);

        // Decrements count of elements by 1
        Size--;
    }
    void deletePageAtRear()
    {
        Node *temp = rear;
        rear = rear->prev;

        // If only one element was present
        if (rear == NULL)
            front = NULL;
        else
            rear->next = NULL;
        free(temp);

        // Decrements count of elements by 1
        Size--;
    }
    int deletePage(void *page)
    {
        // If deque is empty then
        // 'Underflow' condition
        if (isEmpty())
            return -1; 

        if (front->pageAddress == page)
        {
            deletePageAtFront();
        }
        else if (rear->pageAddress == page)
        {
            deletePageAtRear();
        }
        else
        {
            Node *current = front; 
            while(current->pageAddress != page)
                current = current->next; 

            current->prev->next = current->next; 
            delete current; 
            Size--;
        } 
    }
    void* getPageAtFront()
    {
        // If deque is empty, then returns
        // garbage value
        if (isEmpty())
            return (void*)-1;
        return front->pageAddress;
    }
    void* getPageAtRear()
    {
        // If deque is empty, then returns
        // garbage value
        if (isEmpty())
            return (void*)-1;
        return rear->pageAddress;
    }
    int size()
    {
        return Size;
    }
    bool isEmpty()
    {
        return (front == NULL);
    }
    void display()
    {
        Node *current = front; 
        while(current != NULL)
        {
            cout << current->pageAddress << endl;
            current = current->next;
        }
    }
    void erase()
    {
        rear = NULL;
        while (front != NULL)
        {
            Node *temp = front;
            front = front->next;
            free(temp);
        }
        Size = 0;
    }
};

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

class VirtualMem
{
public:
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
    Deque AccessQueue; 
};

#endif