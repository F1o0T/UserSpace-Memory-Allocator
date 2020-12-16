#ifndef MappedChunk_h
#define MappedChunk_h

#include "system/Memory.h"
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <list>

using namespace std;

enum access_flag: int{
    NON = 0,
    READ,
    WRITTEN
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
	QNode *front, *rear; 
    unsigned CurrentQueueSize = 0;
public:
    Queue() 
    { 
        front = rear = NULL; 
    } 
  
    void enQueue(void* x) 
    { 
        // Create a new LL node 
        QNode* temp = new QNode(x); 
  
        // If queue is empty, then 
        // new node is front and rear both 
        if (rear == NULL) { 
            front = rear = temp;
            CurrentQueueSize++; 
            return; 
        } 
 
        // Add the new node at 
        // the end of queue and change rear 
        rear->next = temp; 
        rear = temp; 
        CurrentQueueSize++;
    } 
  
    // Function to remove 
    // a key from given queue q 
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
        CurrentQueueSize--;
        return ptr;
    } 
    bool isFull(unsigned MaxSize)
    {
    	if(CurrentQueueSize >= MaxSize)
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
    void displayQueue()
    {
    	QNode* temp = front;
    	cout << "Active chunks displayed in FirstIn/FirstOut" << endl;
    	while(temp->next != NULL)
    	{
    		cout << reinterpret_cast<unsigned long>(temp->address) << " <= ";
    		temp = temp->next; 
    	}
    	cout << reinterpret_cast<unsigned long>(temp->address);
    	cout << endl;
    }


    void decreaseAccessLevel(void* nodeStartAddress)    
    {
        QNode *currNode = this->front;
        while(currNode->address != nodeStartAddress)
        {
            currNode = currNode->next;
        }
        if(currNode->acc_flag == NON)
        {
            currNode->acc_flag = READ;
        }else
        {
            currNode->acc_flag = WRITTEN;
        }
    }

    //0 = NON, 1 = READ, 2 = WRITTEN
    int getAccessLevel(void * nodeStartAddress)
    {
        QNode *currNode = this->front;

        if (currNode == 0) {
            return 0;
        } 

        while(nodeStartAddress != currNode->address)
        {
            currNode = currNode->next;
            if (currNode == 0) {
                return 0;
            }
        }

        return currNode->acc_flag;

    }


}; 

class MappedChunk {
public:
	/////////////////////////////////////////////////
	// Signal handeler, constructor and deconstructor.
	static void SignalHandeler(int SigNumber, siginfo_t *info, void *ucontext);
	MappedChunk(size_t startSize, size_t blocks, size_t maxactBlocks);
	~MappedChunk();
	/////////////////////////////////////////////////
	// Basic Methods
	void* getStart();
	size_t getSize();
	void* expand(size_t size);
	/////////////////////////////////////////////////
	// Advanced Methods
	void FixPermissions(void*);
	void* FindStartAddress(void* ptr);
    int getAccessLevel(void* ptr);
	void printChunkStarts();
	void DisplayActiveChunks();
    void fillList(list<int>* list);
	/////////////////////////////////////////////////
private:
	void* memblock = NULL;
	size_t chunksNumber = 0;
	size_t maxActChunks = 0 ;
	size_t chunkSize = 0;
	Queue ChunkQueue;
};
#endif