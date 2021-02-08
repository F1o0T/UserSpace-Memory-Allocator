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

enum permission_change: int{
    NONTOREAD_NOTFULL,
    NONTOREAD_FULL,
    READTOWRITE
};

struct QNode { 
    void* address;
    QNode* next;

    QNode(void* add) 
    { 
        address = add;
        next = NULL;
    } 
}; 
  
class Queue 
{ 
	QNode *front, *rear, *previousRear; 
    unsigned currentQueueSize = 0;
    void * lastEnqueuedAddress; 

public:
    Queue() 
    { 
        front = rear = NULL; 
    } 
  
    void enQueue(void* x) 
    { 
        lastEnqueuedAddress = x;
        // Create a new LL node 
        QNode* temp = new QNode(x); 
  
        // If queue is empty, then 
        // new node is front and rear both 
        if (rear == NULL) { 
            front = rear = temp;
            currentQueueSize++; 
            return; 
        } 
 
        // Add the new node at 
        // the end of queue and change rear 
        previousRear = rear;
        rear->next = temp; 
        rear = temp; 
        currentQueueSize++;
    } 
  
    void* deQueue() 
    { 
        // If queue is empty, return NULL. 
        if (front == NULL) 
            return NULL; 
  
        // Store previous front and 
        // move front one node ahead 
        QNode* temp = front;
        front = front->next; 
  
        // If front becomes NULL, then 
        // change rear also as NULL 
        if (front == NULL) 
            rear = NULL; 
        void * ptr = temp-> address;
        delete (temp); 
        currentQueueSize--;
        return ptr;
    } 

    void* deQueue(void* addr)
    {   

        QNode* predTemp = NULL;
        QNode* temp = this->front;
        QNode* succTemp = temp->next;



        while(temp->address != addr && temp->address != NULL)
        {
            predTemp = temp;
            temp = temp->next;
            succTemp = temp->next;
        }

        if(this->front == temp){
            this->front = temp->next;
        }

        if(this->previousRear == temp)
        {
            this->previousRear = predTemp;
        }


        if(this->rear == temp){
            this->rear = predTemp;
        }


        if(predTemp != NULL){
            predTemp->next = succTemp;
        }

        currentQueueSize--;

        return temp->address;
    }

    bool isFull(unsigned MaxSize)
    {
    	if(currentQueueSize >= MaxSize)
    		return true; 
    	else 
    		return false;
    }

    bool isEmpty()
    {
    	if(rear == NULL)
    		return true; 
    	else 
    		return false; 
    }

    void* getLastEnqueuedAddress()
    {
        return lastEnqueuedAddress; 
    }

    void dropLastEnqueuedAddress()
    {   
        this->currentQueueSize--;
        QNode* temp = rear;
        this->rear = this->previousRear;
        delete(temp);
        if(rear == NULL)
        {   
            this->front = NULL;  
            // this->displayQueue();
            return;
        }
        this->rear->next = NULL; 
        // this->displayQueue();

    }

    void displayQueue()
    {
    	QNode* temp = front;
    	cout << "Active chunks displayed in FirstIn/FirstOut" << endl;
    	while(temp != NULL)
    	{
    		cout << reinterpret_cast<unsigned long>(temp->address) << " <= ";
    		temp = temp->next; 
    	}
    	cout << endl;
    }

    int size()
    {
        return currentQueueSize; 
    }
};

class SwapFile: public RandomAccessFile
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

        ~SwapFile() {
            close(fd);
            if (unlink("SwapFile") == -1) {
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
        virtual ssize_t swapFileRead(void* addr, off_t offset, size_t bytes)
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
        virtual ssize_t swapFileWrite(void* addr, off_t offset, size_t bytes)
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

class VirtualMem {
public:
	/////////////////////////////////////////////////
	// Signal handeler, constructor and deconstructor.
	// static void signalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
	// VirtualMem(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
    void initializeVirtualMem();
	~VirtualMem();
	/////////////////////////////////////////////////
	// Basic Methods
	void* getStart();
	size_t getSize();
	void* expand(size_t size);
	/////////////////////////////////////////////////
	// Advanced Methods
	void fixPermissions(void*);
    void initializePDandFirstPT();
    void initializePT(void* startAdress);
    void addPageEntry2PT(unsigned* pageStartAddress);
    void addPTEntry2PD(unsigned* startAddrPage);
	void* findStartAddress(void* ptr);
    void readPageActivate(void* ptr);
    void writePageActivate(void* ptr);
    void pageOut(void* ptr); 
    void pageIn(void* ptr);
    void mapOut(void* pageStartAddress);
    void mapIn(void* pageStartAddress);
	void printChunkStarts();
    void fillList(list<int>* virtualMem, list<unsigned>* physicalMem);
    void resetQueues();
	/////////////////////////////////////////////////
private:
	unsigned* virtualMemStartAddress = NULL;
    unsigned nextFreeFrameIndex = 0;
    unsigned pageoutPointer = 0;
    int fd = 0;
    AddressMapping mappingUnit;
    size_t pinnedPages = 0;
    Queue readQueue;
    Queue writeQueue;
    SwapFile swapFile;
};

#endif