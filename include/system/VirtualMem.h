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

enum access_flag: int{
    NON = 0,
    READ,
    WRITTEN
};
enum swapped_flag: bool
{
    SWAPPED = 1, 
    NON_SWAPPED = 0
};

enum pinned_flag: bool
{
    PINNED = 1,
    NOT_PINNED = 0
};

struct chunkInfo
{
    access_flag accessFlag;
    swapped_flag swapFlag;
    pinned_flag pinnedFlag;
};

struct QNode { 
    void* address;
    access_flag acc_flag;
    QNode* next;

    QNode(void* add) 
    { 
        address = add;
        acc_flag = NON;
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
    void initializeVirtualMem(size_t pinnedChunks, bool writeBackAll);
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
    void addPhysAddresstoPD(void* pageStartAddress);
	void* findStartAddress(void* ptr);
    void kickedPageDeactivate(void* ptr);
    void readPageActivate(void* ptr);
    void writePageActivate(void* ptr);
    void pageOut(void* ptr); 
    void pageIn(void* ptr);
    void mapOut(void* pageStartAddress);
    void mapIn(void* pageStartAddress);
	void printChunkStarts();
	void displayChunks();
    void fillList(list<int>* list);
    void decreaseMaxActChunks(unsigned subtrahend);
    void resetQueues();
    void unpinPage(void* ptr);
    void markPinnedChunks(size_t numberOfChunksToPin);
    void pinOnePage(size_t chunkStartAddr);
	/////////////////////////////////////////////////
private:
	void* virtualMemStartAddress = NULL;
    int fd = 0;
    AddressMapping mappingUnit;
    size_t pinnedPages = 0;
    size_t currentActChunks = 0;
    Queue readQueue;
    Queue writeQueue;
    //Queue pinnedQueue;
    SwapFile swapFile;
    map<size_t, struct chunkInfo>pageInformation;
    bool writeBackAll;
};

#endif