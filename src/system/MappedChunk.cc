#include "system/MappedChunk.h"


#define PAGESIZE sysconf(_SC_PAGESIZE)

void MappedChunk::mappedChunkSet(size_t chunkSize, size_t chunksNumber, size_t pinnedChunks, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll)
{   
    /////////////////////////////////////////////////
    this->chunksNumber = chunksNumber;
    this->maxActChunks = maxChunksAvailable;
    this->chunkSize = chunkSize;
    size_t totalSize = chunksNumber * chunkSize;
    /////////////////////////////////////////////////
    /*Mapping the whole size*/
    this->memBlockStartAddress = mmap(NULL, totalSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(this->memBlockStartAddress == MAP_FAILED)
    {
        cerr << "|###> Error: Mmap Failed" <<endl;
        exit(1); 
    }
    //else 
        //cout << "|###> An anonymous mapping with length = " << totalSize << " has been created" <<endl; 
    /////////////////////////////////////////////////
    /*Seperating the (same size) chunks by marking each one as PROT_NONE*/
    size_t startAddress   =  reinterpret_cast<size_t>(this->memBlockStartAddress); 
    size_t lastAddress    =  startAddress + this->chunkSize * this->chunksNumber; 
    for(size_t chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += this->chunkSize)
    {
        struct chunkInfo chInfo; 
        chInfo.accessFlag = NON;
        chInfo.swapFlag = NON_SWAPPED;
        this->chunksInformation[chunkStartAddress] = chInfo;
        mprotect((void*) chunkStartAddress, this->chunkSize, PROT_NONE);
        //cout << "|###> " << chunkStartAddress << " has been marked with PROT_NONE" << endl;
    }

	if(this->chunksNumber >= this->maxActChunks && this->maxActChunks > pinnedChunks)
	{
		markPinnedChunks(pinnedChunks);
		this->maxActChunks -= pinnedChunks;
	}else{
		cerr << "chunksNumber can't be smaller than max + pin" << endl;
		_exit(1);
	}

	this -> writeBackAll = writeBackAll;
}

void MappedChunk::fixPermissions(void *address)
{
    void* chunkStartAddr = this->findStartAddress(address);

    ////////////////////////////////////////////////////////////////////////
	// remove this chunck from the readQueue if it is there
	if(!this->readQueue.isEmpty())
	{
		
		if(readQueue.getLastEnqueuedAddress() == chunkStartAddr)
		{
			readQueue.dropLastEnqueuedAddress();
			this->currentActChunks--;
		}
	}
	//////////////////////////////////////////////////////////////////////// 
	//cout << writeQueue.size() << endl;
    size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
    int accessFlag = chunksInformation[chunkStartAddress].accessFlag;
    bool swapFlag = this->chunksInformation[chunkStartAddress].swapFlag;

    if(this->currentActChunks != this->maxActChunks)
    {
    	if(accessFlag == NON)
    	{
			if (swapFlag == SWAPPED) 
			{
				this->swapIn((void*)chunkStartAddress);
			}

			readChunkActivate((void*)chunkStartAddress);
    	}	
		else if(accessFlag == READ) // read wird nicht entfernt
		{
			this->chunksInformation[chunkStartAddress].swapFlag = NON_SWAPPED;

			writeChunkActivate((void*)chunkStartAddress);
		}
	}
	else
	{
		// cout << "=================The limit had been reached ===========================" << endl;
		// cout << "ReadQueue Size is " << this->readQueue.size() << endl; 
		// this->readQueue.displayQueue();
		if(!this->readQueue.isEmpty())
		{
			void* kickedReadChunkAddr = this->readQueue.deQueue();
			if(writeBackAll)
			{
				this->chunksInformation[reinterpret_cast<size_t> (kickedReadChunkAddr)].swapFlag = SWAPPED;
				this->swapOut(kickedReadChunkAddr);

			}
			this -> currentActChunks--;
			kickedChunkDeactivate(kickedReadChunkAddr);
			

			if(accessFlag == NON)
			{
				readChunkActivate((void*)chunkStartAddress);
			}
			else if (accessFlag == READ)
			{
				writeChunkActivate((void*)chunkStartAddress);
			}
		}
		else
		{
			// cout << "################ReadQueue is isEmpty#####################" << endl;
			void* kickedWriteChunkAddr = this->writeQueue.deQueue();
			this -> currentActChunks--;
			this->swapOut(kickedWriteChunkAddr);
			kickedChunkDeactivate(kickedWriteChunkAddr);

			if(accessFlag == NON)
			{
				if(swapFlag == SWAPPED)
				{
					this->swapIn((void*)chunkStartAddress);
				}
				
				readChunkActivate((void*)chunkStartAddress);
				
			}
			else if(accessFlag == READ)
			{
				this->chunksInformation[chunkStartAddress].swapFlag = NON_SWAPPED;

				writeChunkActivate((void*)chunkStartAddress);
			}
		}
	}
}


void MappedChunk::kickedChunkDeactivate(void* kickedChunkAddr)
{
	mprotect(kickedChunkAddr, this->chunkSize, PROT_NONE);
	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
    this->chunksInformation[kickedChunkAddress].accessFlag = NON;
}

void MappedChunk::readChunkActivate(void* chunkStartAddr)
{
	mprotect(chunkStartAddr, this->chunkSize, PROT_READ);
	this->readQueue.enQueue(chunkStartAddr);
	this -> currentActChunks++;

	size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
	this->chunksInformation[chunkStartAddress].accessFlag = READ;
}

void MappedChunk::writeChunkActivate(void* chunkStartAddr)
{
	mprotect(chunkStartAddr, this->chunkSize, PROT_WRITE);
	this->writeQueue.enQueue(chunkStartAddr);
	this -> currentActChunks++;

	size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
	this->chunksInformation[chunkStartAddress].accessFlag = WRITTEN;
}

void MappedChunk::markPinnedChunks(size_t numberOfChunksToPin)
{
	size_t chunckStartAddress = reinterpret_cast<size_t> (this->memBlockStartAddress);
	for(size_t i = 0; i < numberOfChunksToPin; i++)
	{
		pinOneChunk(chunckStartAddress);
		chunckStartAddress += this->chunkSize;
	}
}



void MappedChunk::pinOneChunk(size_t chunkStartAddr) {
	this->chunksInformation[chunkStartAddr].pinnedFlag = PINNED;
	this->chunksInformation[chunkStartAddr].accessFlag = WRITTEN;
	mprotect(reinterpret_cast<void*>(chunkStartAddr), this->chunkSize, PROT_READ | PROT_WRITE);
	this->pinnedChunks++;
}

void MappedChunk::unpinChunk(void* chunkStartAddr) {
	
	/*
	pinnedQueue.deQueue(chunkStartAddr);
	this -> currentActChunks--;
	kickedChunkDeactivate(chunkStartAddr);
	*/
}

void MappedChunk::swapOut(void* kickedChunkAddr)
{
	off_t offset = reinterpret_cast<off_t>(kickedChunkAddr)-reinterpret_cast<off_t>(this->memBlockStartAddress); 
	this->swapFile.swapFileWrite(kickedChunkAddr, offset , this->chunkSize);

	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
	this->chunksInformation[kickedChunkAddress].swapFlag = SWAPPED;

	//cout << kickedChunkAddress << " chunck has been swappedOut" << endl;
}
void MappedChunk::swapIn(void* chunckStartAddr)
{
	off_t offset = reinterpret_cast<off_t>(chunckStartAddr)-reinterpret_cast<off_t>(this->memBlockStartAddress); 
	this->swapFile.swapFileRead(chunckStartAddr, offset , this->chunkSize);

	//size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	//cout << chunckStartAddress << " chunck has been swappedIn" << endl;
}

void* MappedChunk::findStartAddress(void * chunckStartAddress)
{
   size_t address        =  reinterpret_cast<size_t>(chunckStartAddress); 
   size_t startAddress   =  reinterpret_cast<size_t>(this->memBlockStartAddress); 
   size_t lastAddress    =  startAddress + this->chunkSize * this->chunksNumber; 
   for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += this->chunkSize)
   {
        if(address >= chunkStartAddress && address < chunkStartAddress + chunkSize) 
            return (void*) chunkStartAddress; 
   }
   /*
    It will never reach here
   */
   return NULL; 
}


void* MappedChunk::getStart()
{

    return this->memBlockStartAddress;
}

size_t MappedChunk::getSize()
{

    return chunkSize * chunksNumber;
}

void MappedChunk::printChunkStarts()
{
    for(unsigned int i = 0; i < this -> chunksNumber; i++)
    {
        cout << "chunk " << i << " : " << reinterpret_cast<unsigned long>(memBlockStartAddress) + i * this->chunkSize <<endl;
    }
}

void MappedChunk::displayChunks()
{
	size_t startAddress   =  reinterpret_cast<size_t>(this->memBlockStartAddress); 
	size_t lastAddress    =  startAddress + this->chunkSize * this->chunksNumber; 
	int count = 0; 
	for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += this->chunkSize)
	{
	    cout << count << "   " << chunkStartAddress << "\t" << this->chunksInformation[chunkStartAddress].accessFlag << "\t" << this->chunksInformation[chunkStartAddress].swapFlag << endl;
		count++;
		cout << endl;
	}	
}

void MappedChunk::fillList(list<int>* list) {
    char* ptr1 = (char*) memBlockStartAddress;
    unsigned size = chunkSize * chunksNumber;
    unsigned i = 0;

    while (size > i) {
        //fill list

        // 0 or 1 or 2 = none or read or write
        size_t chunkStartAddress = reinterpret_cast<size_t>(ptr1);
        list->push_back(this->chunksInformation[chunkStartAddress].accessFlag);

        //move on ptr
        ptr1 += chunkSize;
        i += chunkSize;
    }
}

void* MappedChunk::expand(size_t size)
{
    /*
    i dont think that it makes sense to implement expand because you could do 3 things:
        -increase the number of chunks, but as long as you dont increase the number of max. act. chunks
        it wouldnt have any effect other than increasing the "amount of space in the hard drive"
            (this is what the inactive chungs simulate)
        -increase the amount of max. active chunks...yeah...
        -increase the space of each chunk, but because the chunks are consecutive to each other you first have to destroy
        (at least) all but the first one, increase the memory of the first one for some amount and than rebuild the other with
        the new memory size...this also dont seem like it makes sense for this simulation
    */
    return NULL;
}

MappedChunk::~MappedChunk()
{

    munmap(this->memBlockStartAddress, this->chunksNumber * this -> chunkSize);
}

void MappedChunk::resetQueues() {
	int k = writeQueue.size();
	int l = readQueue.size();
	for (int i = 0; i < k; i++) {
		void* kickedWriteChunkAddr = this->writeQueue.deQueue();
		this -> currentActChunks--;
		kickedChunkDeactivate(kickedWriteChunkAddr);
	}

	for (int i = 0; i < l; i++) {
		void* kickedReadChunkAddr = this->readQueue.deQueue();
		this -> currentActChunks--;
		kickedChunkDeactivate(kickedReadChunkAddr);
	}

	this -> pinnedChunks = 0;
}
