#include "system/VirtualMem.h"


#define PAGESIZE sysconf(_SC_PAGESIZE)
#define FOUR_GB 4294967296
#define PAGETABLE_SIZE 1024
#define NUMBER_OF_PAGES FOUR_GB/PAGESIZE
#define NUMBER_OF_PT NUMBER_OF_PAGES/PAGETABLE_SIZE
#define NUMBER_OF_PAGEFRAMES 10
#define PHY_MEM_LENGTH PAGESIZE*NUMBER_OF_PAGEFRAMES

void VirtualMem::virtualMemSet(size_t pinnedChunks, bool writeBackAll)
{
	this -> fd = shm_open("phy-Mem", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		cerr << "|###> Error: open the shm failed" <<endl;
        exit(1); 
	}
	if (ftruncate(fd, PHY_MEM_LENGTH) == -1) {
		cerr << "|###> Error: truncate failed" <<endl;
        exit(1); 
	}

    /*Mapping the whole size*/
	this -> virtualMemStartAddress = mmap(NULL, FOUR_GB, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(this->virtualMemStartAddress == MAP_FAILED)
    {
        cerr << "|###> Error: virtual Mmap Failed" <<endl;
        exit(1);
    }

	if(NUMBER_OF_PAGES >= NUMBER_OF_PAGEFRAMES && NUMBER_OF_PAGEFRAMES > pinnedChunks)
	{
		markPinnedChunks(pinnedChunks);
		//NUMBER_OF_PAGEFRAMES -= pinnedChunks;
	}else{
		cerr << "NUMBER_OF_PAGES can't be smaller than max + pin" << endl;
		_exit(1);
	}

	munmap(this->virtualMemStartAddress, PAGESIZE*(NUMBER_OF_PT+1));

	//Page-Directory
	char* addrPD = (char*) mmap(this->virtualMemStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FIXED, this -> fd, 0);
    if(addrPD == MAP_FAILED)
    {
        cerr << "|###> Error: Mmap PD Failed" <<endl;
        exit(1);
    }

	//first Page-Table 
	char* addrFirstPT = (char*) mmap(((char*) this->virtualMemStartAddress) + PAGESIZE, PAGESIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FIXED, this -> fd, PAGESIZE);
    if(addrFirstPT == MAP_FAILED)
    {
        cerr << "|###> Error: Mmap first PT Failed" <<endl;
        exit(1);
    }
	cout << "addrPD = " << (void*) addrPD << endl;
	cout << "addrFirstPT = " << (void*) addrFirstPT << endl;

	unsigned phyaddrFirstPT = addrFirstPT - addrPD;//4096
	*(reinterpret_cast<unsigned*>(addrPD)) = phyaddrFirstPT;
	cout << "phyaddrFirstPT = " << phyaddrFirstPT << endl;

	unsigned phyaddrPD = addrPD - addrPD;//0
	unsigned* firstPTentry = reinterpret_cast<unsigned*>(addrFirstPT);

	cout << "firstPTentry = " << firstPTentry << endl;
	cout << "phyaddrPD = " << phyaddrPD << endl;
	*(firstPTentry) = phyaddrPD;
	cout << "firstPTentry1 = " << *(firstPTentry) << endl;
	firstPTentry++;
	*(firstPTentry) = phyaddrFirstPT;
	cout << "firstPTentry2 = " << *(firstPTentry) << endl;
}

void VirtualMem::fixPermissions(void *address)
{
	void* chunkStartAddr = this->findStartAddress(address);
    size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
    int accessFlag = chunksInformation[chunkStartAddress].accessFlag;
    bool swapFlag = this->chunksInformation[chunkStartAddress].swapFlag;

	permission_change permissionChange;

	//determine which permission change is the right one
	if(accessFlag == NON)
	{
		//this is the case when we change the permission from non to read
		if(this->currentActChunks < NUMBER_OF_PAGEFRAMES)
		{
			permissionChange = NONTOREAD_NOTFULL;
		}else
		{
			permissionChange = NONTOREAD_FULL;
		}
	}else{
		//this is the case when we change the permission from read to write
			permissionChange = READTOWRITE;
	}


	switch(permissionChange)
	{
		case NONTOREAD_NOTFULL:
			//if there is already data on the disk, we have to this data in
			if(swapFlag == SWAPPED){
				this->pageIn(chunkStartAddr);
			}
			readChunkActivate(chunkStartAddr);
			break;
		case NONTOREAD_FULL:
			//first we have to kick out one chunk, preferibly in the readQueue
			void* kickedChunkAddr;
			if(!this->readQueue.isEmpty()){
				kickedChunkAddr = this->readQueue.deQueue();
				//if writeBackAll is activated, we have to write this chunk on the disk
				if(writeBackAll){
					this->pageOut(kickedChunkAddr);
				}
			}else{
				//kickout a chunk in the writeQueue
				kickedChunkAddr = this->writeQueue.deQueue();
				this->pageOut(kickedChunkAddr);
			}
			//now just deactivate all the stuff
			kickedChunkDeactivate(kickedChunkAddr);

			//last but not least: activate the chunk just like in case 1
			if(swapFlag == SWAPPED){
				this->pageIn(chunkStartAddr);
			}
			//
			readChunkActivate(chunkStartAddr);
			break;
		case READTOWRITE:
			//first we have to delete the chunk in the read queue
			this->readQueue.deQueue(chunkStartAddr);
			//last we set all the flags right
			writeChunkActivate(chunkStartAddr);
			break;
	}
}


void VirtualMem::kickedChunkDeactivate(void* kickedChunkAddr)
{
	mprotect(kickedChunkAddr, PAGESIZE, PROT_NONE);
	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
    this->chunksInformation[kickedChunkAddress].accessFlag = NON;
	this->currentActChunks--;
}

void VirtualMem::readChunkActivate(void* chunkStartAddr)
{
	mprotect(chunkStartAddr, PAGESIZE, PROT_READ);
	this->readQueue.enQueue(chunkStartAddr);
	this -> currentActChunks++;

	size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
	this->chunksInformation[chunkStartAddress].accessFlag = READ;
}

void VirtualMem::writeChunkActivate(void* chunkStartAddr)
{
	mprotect(chunkStartAddr, PAGESIZE, PROT_WRITE);
	this->writeQueue.enQueue(chunkStartAddr);

	size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
	this->chunksInformation[chunkStartAddress].accessFlag = WRITTEN;
	this->chunksInformation[chunkStartAddress].swapFlag = NON_SWAPPED;
}

void VirtualMem::markPinnedChunks(size_t numberOfChunksToPin)
{
	size_t chunckStartAddress = reinterpret_cast<size_t> (this->virtualMemStartAddress);
	for(size_t i = 0; i < numberOfChunksToPin; i++)
	{
		pinOneChunk(chunckStartAddress);
		chunckStartAddress += PAGESIZE;
	}
}



void VirtualMem::pinOneChunk(size_t chunkStartAddr) {
	this->chunksInformation[chunkStartAddr].pinnedFlag = PINNED;
	this->chunksInformation[chunkStartAddr].accessFlag = WRITTEN;
	mprotect(reinterpret_cast<void*>(chunkStartAddr), PAGESIZE, PROT_READ | PROT_WRITE);
	this->pinnedChunks++;
}

void VirtualMem::unpinChunk(void* chunkStartAddr) {
	
	/*
	pinnedQueue.deQueue(chunkStartAddr);
	this -> currentActChunks--;
	kickedChunkDeactivate(chunkStartAddr);
	*/
}

void VirtualMem::pageOut(void* kickedChunkAddr)
{
	off_t offset = reinterpret_cast<off_t>(kickedChunkAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileWrite(kickedChunkAddr, offset , PAGESIZE);

	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
	this->chunksInformation[kickedChunkAddress].swapFlag = SWAPPED;

	//cout << kickedChunkAddress << " chunck has been swappedOut" << endl;
}
void VirtualMem::pageIn(void* chunckStartAddr)
{
	off_t offset = reinterpret_cast<off_t>(chunckStartAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileRead(chunckStartAddr, offset , PAGESIZE);

	//size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	//cout << chunckStartAddress << " chunck has been swappedIn" << endl;
}

void VirtualMem::mapOut(void* pageStartAddress) {
	//need to write in the dir where the page is
}

void VirtualMem::mapIn(void* pageStartAddress) {
	//unmap the virtualspace 
	munmap(pageStartAddress, PAGESIZE);

	//translate logical to physical address
	//unsigned page = addr2page(pageStartAddress);
	//unsigned phyAddr = page2frame(page);

	//map in the physical space
	//void* addr = mmap(pageStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FIXED, this -> fd, phyAddr * PAGESIZE);
    // if(addr == MAP_FAILED)
    // {
    //     cerr << "|###> Error: phy Mmap Failed" <<endl;
    //     exit(1);
    // }
}

void* VirtualMem::findStartAddress(void * chunckStartAddress)
{
   size_t address        =  reinterpret_cast<size_t>(chunckStartAddress); 
   size_t startAddress   =  reinterpret_cast<size_t>(this->virtualMemStartAddress); 
   size_t lastAddress    =  startAddress + PAGESIZE * NUMBER_OF_PAGES; 
   for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += PAGESIZE)
   {
        if(address >= chunkStartAddress && address < chunkStartAddress + PAGESIZE) 
            return (void*) chunkStartAddress; 
   }
   /*
    It will never reach here
   */
   return NULL; 
}


void* VirtualMem::getStart()
{
    return (void*) (((char*) this->virtualMemStartAddress) + ((NUMBER_OF_PT+1) * PAGESIZE));
}

size_t VirtualMem::getSize()
{

    return PAGESIZE * NUMBER_OF_PAGES;
}

void VirtualMem::printChunkStarts()
{
    for(unsigned int i = 0; i < NUMBER_OF_PAGES; i++)
    {
        cout << "chunk " << i << " : " << reinterpret_cast<unsigned long>(virtualMemStartAddress) + i * PAGESIZE <<endl;
    }
}

void VirtualMem::displayChunks()
{
	size_t startAddress   =  reinterpret_cast<size_t>(this->virtualMemStartAddress); 
	size_t lastAddress    =  startAddress + PAGESIZE * NUMBER_OF_PAGES; 
	int count = 0; 
	for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += PAGESIZE)
	{
	    cout << count << "   " << chunkStartAddress << "\t" << this->chunksInformation[chunkStartAddress].accessFlag << "\t" << this->chunksInformation[chunkStartAddress].swapFlag << endl;
		count++;
		cout << endl;
	}	
}

void VirtualMem::fillList(list<int>* list) {
	list->push_back(2);
	list->push_back(2);
	list->push_back(1);
	list->push_back(1);
	list->push_back(0);
	list->push_back(1);
	list->push_back(1);
	list->push_back(0);
	list->push_back(2);
	list->push_back(1);
	list->push_back(0);
	list->push_back(1);
	list->push_back(2);
	list->push_back(0);
	list->push_back(0);
	list->push_back(1);

    /*char* ptr1 = (char*) virtualMemStartAddress;
    size_t size = PAGESIZE * NUMBER_OF_PAGES;
    size_t i = 0;
	size_t chunkStartAddress;

    while (size > i) {
        //fill list

        // 0 or 1 or 2 = none or read or write
        chunkStartAddress = reinterpret_cast<size_t>(ptr1);
        list->push_back(this->chunksInformation[chunkStartAddress].accessFlag);

        //move on ptr
        ptr1 += PAGESIZE;
        i += PAGESIZE;
    }*/
}

void* VirtualMem::expand(size_t size)
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

VirtualMem::~VirtualMem()
{

    munmap(this->virtualMemStartAddress, NUMBER_OF_PAGES * PAGESIZE);
	shm_unlink("phy-Mem");
}

void VirtualMem::resetQueues() {
	int k = writeQueue.size();
	int l = readQueue.size();
	for (int i = 0; i < k; i++) {
		void* kickedWriteChunkAddr = this->writeQueue.deQueue();
		kickedChunkDeactivate(kickedWriteChunkAddr);
	}

	for (int i = 0; i < l; i++) {
		void* kickedReadChunkAddr = this->readQueue.deQueue();
		kickedChunkDeactivate(kickedReadChunkAddr);
	}

	this -> pinnedChunks = 0;
}
