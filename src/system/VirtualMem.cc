#include "system/VirtualMem.h"


#define PAGESIZE sysconf(_SC_PAGESIZE)
#define FOUR_GB 4294967296
#define PAGETABLE_SIZE 1024
#define NUMBER_OF_PAGES FOUR_GB/PAGESIZE
#define NUMBER_OF_PT NUMBER_OF_PAGES/PAGETABLE_SIZE
#define NUMBER_OF_PAGEFRAMES 10
#define PHY_MEM_LENGTH PAGESIZE*NUMBER_OF_PAGEFRAMES

void VirtualMem::initializeVirtualMem(bool writeBackAll)
{
	//open the shared memory file (physical memory)
	this -> fd = shm_open("phy-Mem", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		cerr << "|###> Error: open the shm failed" <<endl;
        exit(1); 
	}
	if (ftruncate(fd, PHY_MEM_LENGTH) == -1) {
		cerr << "|###> Error: truncate failed" <<endl;
        exit(1); 
	}

    /*map the whole logical memory size*/
	this -> virtualMemStartAddress = (caddr_t) mmap(NULL, FOUR_GB, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(this -> virtualMemStartAddress == (caddr_t) MAP_FAILED)
    {
        cerr << "|###> Error: virtual Mmap Failed" <<endl;
        exit(1);
    }

	initializePDandFirstPT();

	pointerToNextFreeFrame = 2;


	//Debug
	unsigned* start = (unsigned*) virtualMemStartAddress;
	cout << "start: " << reinterpret_cast<size_t> (start) << endl;
	cout << "start content" << (*(start)) << endl;
	cout << "content first entry at first PT: " << (this->virtualMemStartAddress) + (*(start)) << endl;
	cout << "content first entry at first PT: " << (this->virtualMemStartAddress) + (*(start) + 1) << endl;
}


/**
 * This method is just called, when the whole virtual memory gets initialized.
 * It unmaps the first 2 pages of logical memory and maps 2 page frames of phys. memory,
 * to use. Furthermore it pins them.
*/
void VirtualMem::initializePDandFirstPT()
{
	//unmap the first page, to map the same number of page frame for the PD
	//munmap(this->virtualMemStartAddress, PAGESIZE*(NUMBER_OF_PT+1));-> think this is wrong, because we are not using all PT in the beginning
	munmap(this->virtualMemStartAddress, PAGESIZE);

	//map page frame for PD
	unsigned* addrPD = (unsigned*) mmap(this->virtualMemStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_FIXED, this -> fd, 0);
    if(addrPD == (unsigned*) MAP_FAILED)
    {
        cerr << "|###> Error: Mmap PD Failed" <<endl;
        exit(1);
    }

	//book keeping for the activated page -> you have to do both steps because it is not wanted that
	writePageActivate(this->virtualMemStartAddress);
	this->currentActPages++;//unnötig pointerToNextFreeFrame macht das

	
	//initialize first PT with the two phys adresses of the PD and the PT itself
	initializePT((this->virtualMemStartAddress) + PAGESIZE);
	*((this->virtualMemStartAddress) + PAGESIZE) = (0 << 12) | mappingUnit.createOffset(1,1,1,1,0,0);
	*((this->virtualMemStartAddress) + PAGESIZE + 4) = (1 << 12) | mappingUnit.createOffset(1,1,1,1,0,0);



	//add the physical address of the PT in the PD -> here the logical and physical value is equal to one another
	*(addrPD) = (1 << 12) | mappingUnit.createOffset(1,1,1,1,0,0);


	//create all PT but the presentBits are not set -> they are not mapped in
	unsigned offset = mappingUnit.createOffset(0,1,1,1,0,0);
	for(unsigned i = 1; i < PAGETABLE_SIZE; i++)
	{
		//(i+1) because at 0 is PD and at 1 is first PT, so start the others PT starts at 2
		*(addrPD + i) = ((i+1) << 12) | offset;
	}
}


/**
	This method can be used to initialize a PT, which
	is not in the phys. memory yet
	@param startAddr logical start address of the PT
*/
void VirtualMem::initializePT(void *pageStartAddress)
{
	//unmap page of logical memory
	munmap(pageStartAddress, PAGESIZE);
	
	//map page frame for PT 
	caddr_t addrFirstPT = (char*) mmap(pageStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_FIXED, this -> fd, PAGESIZE);
    if(addrFirstPT == MAP_FAILED)
    {
        cerr << "|###> Error: Mmap PT Failed" <<endl;
        exit(1);
    }

	//book keeping for the activated page -> you have to do both steps
	
	writePageActivate(pageStartAddress);
	this->currentActPages++;
}


void VirtualMem::fixPermissions(void *address)
{
	void* chunkStartAddr = this->findStartAddress(address);
    size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
    int accessFlag = pageInformation[chunkStartAddress].accessFlag;
    bool swapFlag = this->pageInformation[chunkStartAddress].swapFlag;

	permission_change permissionChange;

	//determine which permission change is the right one
	if(accessFlag == NON)
	{
		//this is the case when we change the permission from non to read
		if(this->currentActPages < NUMBER_OF_PAGEFRAMES)
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
			if (swapFlag == SWAPPED) {
				this->pageIn(chunkStartAddr);
			} else {
				addPTEntry(chunkStartAddr);
			}
			readPageActivate(chunkStartAddr);
			mapIn(chunkStartAddr);
			break;
		case NONTOREAD_FULL:
			//first we have to kick out one chunk, preferibly in the readQueue
			void* kickedChunkAddr;
			if (!this->readQueue.isEmpty()) {
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
			kickedPageDeactivate(kickedChunkAddr);
			mapOut(kickedChunkAddr);

			//last but not least: activate the chunk just like in case 1
			if(swapFlag == SWAPPED){
				this->pageIn(chunkStartAddr);
			} else {
				addPTEntry(chunkStartAddr);
			}
			//
			readPageActivate(chunkStartAddr);
			mapIn(chunkStartAddr);
			break;
		case READTOWRITE:
			//first we have to delete the chunk in the read queue
			this->readQueue.deQueue(chunkStartAddr);
			//last we set all the flags right
			writePageActivate(chunkStartAddr);
			break;
	}
}

void VirtualMem::addPTEntry(void* startAddrPage) {
	unsigned distanceToPage = ((char*) startAddrPage) - virtualMemStartAddress;
	unsigned first20Bits = mappingUnit.addr2page((caddr_t) distanceToPage);
	unsigned first10Bits = mappingUnit.page2pageDirectoryIndex(first20Bits);
	unsigned second10Bits = mappingUnit.page2pageTableIndex(first20Bits);
	unsigned addrToPT = *(virtualMemStartAddress + first10Bits);

	//calculate the right PT 
	unsigned pageNumber = distanceToPage/PAGESIZE;
	unsigned indexofPT = (pageNumber/PAGETABLE_SIZE);
	unsigned addrOfPT = PAGESIZE*(indexofPT+1);


	//Just to Debug
	unsigned addrToPage = addrToPT + second10Bits;
	cout << "new PTEntry " << addrOfPT << " == " << addrToPage << endl;


	//if the PT not existing then create it by setting the presentBit
	if (mappingUnit.getPresentBit(addrToPT) == 0) {
		*(virtualMemStartAddress + first10Bits) = addrOfPT + mappingUnit.setPresentBit(addrToPT);
	}

	//add the page in PT
	*(virtualMemStartAddress + addrOfPT + second10Bits) = (pointerToNextFreeFrame << 12) | mappingUnit.createOffset(1,0,0,1,0,1);
	pointerToNextFreeFrame++;
}

void VirtualMem::kickedPageDeactivate(void* kickedChunkAddr)
{
	mprotect(kickedChunkAddr, PAGESIZE, PROT_NONE);
	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
    this->pageInformation[kickedChunkAddress].accessFlag = NON;
	this->currentActPages--;
}

void VirtualMem::readPageActivate(void* chunkStartAddr)
{
	mprotect(chunkStartAddr, PAGESIZE, PROT_READ);
	this->readQueue.enQueue(chunkStartAddr);
	this -> currentActPages++;

	size_t chunkStartAddress = reinterpret_cast<size_t> (chunkStartAddr);
	this->pageInformation[chunkStartAddress].accessFlag = READ;
}

void VirtualMem::writePageActivate(void* pageStartAddr)
{
	mprotect(pageStartAddr, PAGESIZE, PROT_WRITE);
	this->writeQueue.enQueue(pageStartAddr);

	size_t pageStartAddress = reinterpret_cast<size_t> (pageStartAddr);
	this->pageInformation[pageStartAddress].accessFlag = WRITTEN;
	this->pageInformation[pageStartAddress].swapFlag = NON_SWAPPED;
}

void VirtualMem::pinOnePage(size_t chunkStartAddr) {
	this->pageInformation[chunkStartAddr].pinnedFlag = PINNED;
	this->pageInformation[chunkStartAddr].accessFlag = WRITTEN;
	mprotect(reinterpret_cast<void*>(chunkStartAddr), PAGESIZE, PROT_READ | PROT_WRITE);
	this->pinnedPages++;
}


void VirtualMem::pageOut(void* kickedChunkAddr)
{
	off_t offset = reinterpret_cast<off_t>(kickedChunkAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileWrite(kickedChunkAddr, offset , PAGESIZE);

	size_t kickedChunkAddress = reinterpret_cast<size_t> (kickedChunkAddr);
	this->pageInformation[kickedChunkAddress].swapFlag = SWAPPED;

	
	// PT handling
	caddr_t caddPageStartAddress = (caddr_t) ((char*) kickedChunkAddr - virtualMemStartAddress);
	unsigned first20Bits = mappingUnit.addr2page(caddPageStartAddress);
	unsigned first10Bits = mappingUnit.page2pageDirectoryIndex(first20Bits);
	unsigned second10Bits = mappingUnit.page2pageTableIndex(first20Bits);

	unsigned addrToPT = *(virtualMemStartAddress + first10Bits);

}

void VirtualMem::pageIn(void* chunckStartAddr)
{
	off_t offset = reinterpret_cast<off_t>(chunckStartAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileRead(chunckStartAddr, offset , PAGESIZE);

	//size_t chunckStartAddress = reinterpret_cast<size_t> (chunckStartAddr);
	//cout << chunckStartAddress << " chunck has been swappedIn" << endl;
}

void VirtualMem::mapOut(void* pageStartAddress) {
	//map out shared memory file
	munmap(pageStartAddress, PAGESIZE);

	//map in MAP_Anonymous (simulation for no physical nemory behind it)
	mmap(pageStartAddress, PAGESIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
}

void VirtualMem::mapIn(void* pageStartAddress) {
	size_t decPageStartAddress = reinterpret_cast<size_t> (pageStartAddress);
	caddr_t caddPageStartAddress = reinterpret_cast<caddr_t> (pageStartAddress);

	//unmap the virtual space
	munmap(pageStartAddress, PAGESIZE);
	
	//translate logical to physical address
	unsigned phyAddr = mappingUnit.log2phys((unsigned*) virtualMemStartAddress, caddPageStartAddress);
	cout << "mapin phyAddr of Page: " << phyAddr << endl;

	//map in the physical space
	void* addr;
	if (this->pageInformation[decPageStartAddress].accessFlag == NON) {
		addr = mmap(pageStartAddress, PAGESIZE, PROT_READ, MAP_PRIVATE | MAP_FIXED, this -> fd, phyAddr);

	} else if (this->pageInformation[decPageStartAddress].accessFlag == READ) {
		addr = mmap(pageStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_FIXED, this -> fd, phyAddr);
	}

    if(addr == MAP_FAILED) {
    	cerr << "|###> Error: phy Mmap Failed from " << pageStartAddress << " to " << phyAddr << endl;
    	exit(1);
    }
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
    return (void*) (this->virtualMemStartAddress + ((NUMBER_OF_PT+1) * PAGESIZE));
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
	    cout << count << "   " << chunkStartAddress << "\t" << this->pageInformation[chunkStartAddress].accessFlag << "\t" << this->pageInformation[chunkStartAddress].swapFlag << endl;
		count++;
		cout << endl;
	}	
}

void VirtualMem::fillList(list<int>* list) {
    char* ptr1 = virtualMemStartAddress;
    unsigned size = PAGESIZE * NUMBER_OF_PAGES;
    unsigned i = 0;

    while (size > i) {
        //fill list

        // 0 or 1 or 2 = none or read or write
        size_t chunkStartAddress = reinterpret_cast<size_t>(ptr1);
        list->push_back(this->pageInformation[chunkStartAddress].accessFlag);

        //move on ptr
        ptr1 += PAGESIZE;
        i += PAGESIZE;
    }
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
		kickedPageDeactivate(kickedWriteChunkAddr);
	}

	for (int i = 0; i < l; i++) {
		void* kickedReadChunkAddr = this->readQueue.deQueue();
		kickedPageDeactivate(kickedReadChunkAddr);
	}

	this -> pinnedPages = 0;
}
