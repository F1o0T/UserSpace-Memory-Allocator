#ifndef MappedChunk_h
#define MappedChunk_h

#include "system/Memory.h"
#include "misc/RandomAccessFile.h"
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

struct chunckInfo
{
    access_flag accessFlag;
    swapped_flag swapFlag; 
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

    // void decreaseAccessLevel(void* nodeStartAddress)    
    // {
    //     QNode *currNode = this->front;
    //     while(currNode->address != nodeStartAddress)
    //     {
    //         currNode = currNode->next;
    //     }
    //     if(currNode->acc_flag == NON)
    //     {
    //         currNode->acc_flag = READ;
    //     }else
    //     {
    //         currNode->acc_flag = WRITTEN;
    //     }
    // }

    //0 = NON, 1 = READ, 2 = WRITTEN
    // int getAccessLevel(void * nodeStartAddress)
    // {
    //     QNode *currNode = this->front;

    //     if (currNode == 0) {
    //         return 0;
    //     } 

    //     while(nodeStartAddress != currNode->address)
    //     {
    //         currNode = currNode->next;
    //         if (currNode == 0) {
    //             return 0;
    //         }
    //     }

    //     return currNode->acc_flag;

    // }
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
            unlink("SwapFile");
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

class MappedChunk {
public:
	/////////////////////////////////////////////////
	// Signal handeler, constructor and deconstructor.
	// static void signalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
	// MappedChunk(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
    void mappedChunkSet(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll);
	~MappedChunk();
	/////////////////////////////////////////////////
	// Basic Methods
	void* getStart();
	size_t getSize();
	void* expand(size_t size);
	/////////////////////////////////////////////////
	// Advanced Methods
	void fixPermissions(void*);
	void* findStartAddress(void* ptr);
    void kickedChunkDeactivate(void* ptr);
    void readChunkActivate(void* ptr);
    void writeChunkActivate(void* ptr);
    void swapOut(void* ptr); 
    void swapIn(void* ptr);
	void printChunkStarts();
	void displayChunks();
    void fillList(list<int>* list);
    void decreaseMaxActChunks(unsigned subtrahend);
    void reset();
	/////////////////////////////////////////////////
private:
	void* memBlockStartAddress = NULL;
	size_t chunksNumber = 0;
	size_t maxActChunks = 0 ;
    size_t currentActChuncks = 0; 
	size_t chunkSize = 0;
    Queue readQueue;
    Queue writeQueue; 
    SwapFile swapFile;
    map<size_t, struct chunckInfo>chuncksInformation;
    bool writeBackAll;
};

#endif