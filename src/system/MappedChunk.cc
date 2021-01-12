#include "system/MappedChunk.h"


#define PAGESIZE sysconf(_SC_PAGESIZE)

void MappedChunk::mappedChunkSet(size_t chunkSize, size_t chunksNumber, size_t blockSize, size_t maxChunksAvailable, bool writeBackAll)
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
    else 
        cout << "|###> An anonymous mapping with length = " << totalSize << " has been created" <<endl; 
    /////////////////////////////////////////////////
    /*Seperating the (same size) chunks by marking each one as PROT_NONE*/
    size_t startAddress   =  reinterpret_cast<size_t>(this->memBlockStartAddress); 
    size_t lastAddress    =  startAddress + this->chunkSize * this->chunksNumber; 
    for(size_t chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += this->chunkSize)
    {
        struct chunckInfo chInfo; 
        chInfo.accessFlag = NON;
        chInfo.swapFlag = NON_SWAPPED;
        this->chuncksInformation[chunkStartAddress] = chInfo;
        mprotect((void*) chunkStartAddress, this->chunkSize, PROT_NONE);
        cout << "|###> " << chunkStartAddress << " has been marked with PROT_NONE" << endl;
    }
}

void MappedChunk::fixPermissions(void *address)
{

	//cout << this->currentActChuncks << endl;
    void* chunckStartAddr = this->findStartAddress(address);

    ////////////////////////////////////////////////////////////////////////
	// remove this chunck from the readQueue if it is there
	if(!this->readQueue.isEmpty())
	{
		 
		if(readQueue.getLastEnqueuedAddress() == chunckStartAddr)
		{
			readQueue.dropLastEnqueuedAddress();
			this->currentActChuncks -= 1;
		}
	}
	//////////////////////////////////////////////////////////////////////// 

    size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
    int accessFlag = chuncksInformation[chunckStartAddress].accessFlag;
    bool swapFlag = this->chuncksInformation[chunckStartAddress].swapFlag;

    if(this->currentActChuncks != this->maxActChunks)
    {
    	if(accessFlag == NON)
    	{
    		if(swapFlag == NON_SWAPPED)
    		{
    			readChunkActivate((void*)chunckStartAddress);
    		}
    		else
    		{
    			this->swapIn((void*)chunckStartAddress);
    			readChunkActivate((void*)chunckStartAddress);
    		}
    	}	
		else if(accessFlag == READ)
		{

			if(swapFlag == NON_SWAPPED)
			{
				writeChunkActivate((void*)chunckStartAddress);
			}
			else
			{
				this->swapIn((void*)chunckStartAddress);
				writeChunkActivate((void*)chunckStartAddress);
			}
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
			if(accessFlag == NON)
			{
				kickedChunkDeactivate(kickedReadChunkAddr);
				readChunkActivate((void*)chunckStartAddress);
			}
			else if (accessFlag == READ)
			{
				kickedChunkDeactivate(kickedReadChunkAddr);
				writeChunkActivate((void*)chunckStartAddress);
			}
		}
		else
		{
			// cout << "################ReadQueue is isEmpty#####################" << endl;
			void* kickedWriteChunkAddr = this->writeQueue.deQueue();
			if(accessFlag == NON)
			{
				if(swapFlag == NON_SWAPPED)
				{
					this->swapOut(kickedWriteChunkAddr);
					kickedChunkDeactivate(kickedWriteChunkAddr);
					readChunkActivate((void*)chunckStartAddress);
				}
				else 
				{
					this->swapOut(kickedWriteChunkAddr);
					kickedChunkDeactivate(kickedWriteChunkAddr);

					this->swapIn((void*)chunckStartAddress);
					readChunkActivate((void*)chunckStartAddress);
				}
			}
			else if(accessFlag == READ)
			{
				if(swapFlag == NON_SWAPPED)
				{
					this->swapOut(kickedWriteChunkAddr);
					kickedChunkDeactivate(kickedWriteChunkAddr);
					writeChunkActivate((void*)chunckStartAddress);
				}
				else
				{
					this->swapOut(kickedWriteChunkAddr);
					kickedChunkDeactivate(kickedWriteChunkAddr);

					this->swapIn((void*)chunckStartAddress);
					writeChunkActivate((void*)chunckStartAddress);
				}
			}
		}

	}
}


void MappedChunk::kickedChunkDeactivate(void* kickedChunkAddr)
{
	mprotect(kickedChunkAddr, this->chunkSize, PROT_NONE);
	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
    this->chuncksInformation[kickedChunkAddress].accessFlag = NON;
}

void MappedChunk::readChunkActivate(void* chunckStartAddr)
{
	mprotect(chunckStartAddr, this->chunkSize, PROT_READ);
	this->readQueue.enQueue(chunckStartAddr);

	size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	this->chuncksInformation[chunckStartAddress].accessFlag = READ;
	if(!(this->currentActChuncks >= this->maxActChunks))
		this->currentActChuncks += 1;
	cout << chunckStartAddress << " marked as READ and  currentActChuncks = "<< currentActChuncks << endl;
}

void MappedChunk::writeChunkActivate(void* chunckStartAddr)
{

	mprotect(chunckStartAddr, this->chunkSize, PROT_WRITE);
	this->writeQueue.enQueue(chunckStartAddr);

	size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	this->chuncksInformation[chunckStartAddress].accessFlag = WRITTEN;
	if(!(this->currentActChuncks >= this->maxActChunks))
		this->currentActChuncks += 1;
	cout << chunckStartAddress << " marked as WRITE and currentActChuncks = "<< currentActChuncks << endl;
}


void MappedChunk::swapOut(void* kickedChunkAddr)
{
	off_t offset = reinterpret_cast<off_t>(kickedChunkAddr)-reinterpret_cast<off_t>(this->memBlockStartAddress); 
	this->swapFile.swapFileWrite(kickedChunkAddr, offset , this->chunkSize);

	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
	this->chuncksInformation[kickedChunkAddress].swapFlag = SWAPPED;

	cout << kickedChunkAddress << " chunck has been swappedOut" << endl;
}
void MappedChunk::swapIn(void* chunckStartAddr)
{
	off_t offset = reinterpret_cast<off_t>(chunckStartAddr)-reinterpret_cast<off_t>(this->memBlockStartAddress); 
	this->swapFile.swapFileRead(chunckStartAddr, offset , this->chunkSize);

	size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	this->chuncksInformation[chunckStartAddress].swapFlag = NON_SWAPPED;
	cout << chunckStartAddress << " chunck has been swappedIn" << endl;
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
	int count = 1; 
	for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += this->chunkSize)
	{
	    cout << count << "   " << chunkStartAddress << "\t" << this->chuncksInformation[chunkStartAddress].accessFlag << "\t" << this->chuncksInformation[chunkStartAddress].swapFlag << endl;
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
        list->push_back(this->chuncksInformation[chunkStartAddress].accessFlag);

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


