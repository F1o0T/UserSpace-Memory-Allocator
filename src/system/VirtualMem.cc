#include "system/VirtualMem.h"


#define PAGESIZE sysconf(_SC_PAGESIZE)
#define FOUR_GB 4294967296
#define PAGETABLE_SIZE 1024
#define NUMBER_OF_PAGES FOUR_GB/PAGESIZE
#define NUMBER_OF_PT NUMBER_OF_PAGES/PAGETABLE_SIZE
#define NUMBER_OF_PAGEFRAMES 20
#define PHY_MEM_LENGTH PAGESIZE*NUMBER_OF_PAGEFRAMES

void VirtualMem::initializeVirtualMem()
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
	this -> virtualMemStartAddress = (unsigned*) mmap(NULL, FOUR_GB, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(this -> virtualMemStartAddress == (unsigned*) MAP_FAILED)
    {
        cerr << "|###> Error: virtual Mmap Failed" <<endl;
        exit(1);
    }

	cout << "initMem 1" << endl;
	initializePDandFirstPT();


	//Debug
	cout << "start1: " << virtualMemStartAddress << endl;
	cout << "start2: " << reinterpret_cast<size_t> (virtualMemStartAddress) << endl;
	cout << "start content" << (*(virtualMemStartAddress)) << endl;
	cout << "content first entry at first PT: " << *((this->virtualMemStartAddress) + PAGETABLE_SIZE) << endl;
	cout << "content first entry at first PT: " << *((this->virtualMemStartAddress) + PAGETABLE_SIZE + 1) << endl;
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
	nextFreeFrameIndex++;

	//book keeping for the activated page -> you have to do both steps because it is not wanted that
	this->writeQueue.enQueue(virtualMemStartAddress);

	cout << "initPDandPT" << endl;
	//initialize first PT with the two phys adresses of the PD and the PT itself
	initializePT(virtualMemStartAddress + PAGETABLE_SIZE);
	*(virtualMemStartAddress + PAGETABLE_SIZE) = ((0 << 12) | mappingUnit.createOffset(1,1,1,0));
	*(virtualMemStartAddress + PAGETABLE_SIZE + 1) = ((1 << 12) | mappingUnit.createOffset(1,1,1,0));


	//add the physical address of the PT in the PD -> here the logical and physical value is equal to one another
	*(virtualMemStartAddress) = (1 << 12) | mappingUnit.createOffset(1,1,1,1);


	//create all PT but the presentBits are not set -> they are not mapped in
	unsigned offset = mappingUnit.createOffset(0,1,0,1);
	for(unsigned i = 1; i < PAGETABLE_SIZE; i++)
	{
		//(i+1) because at 0 is PD and at 1 is first PT, so start the others PT starts at 2
		*(virtualMemStartAddress + i) = (((i+1) << 12) | offset);
	}
}


/**
	This method can be used to initialize a PT, which
	is not in the phys. memory yet
	@param startAddr logical start address of the PT
*/
void VirtualMem::initializePT(void *pageStartAddress) //TODO eins rauswerfen wenn rein muss
{
	cout << this->nextFreeFrameIndex << " is smaller than " << NUMBER_OF_PAGEFRAMES << endl;
	//if the RAM is full we need to throw something out
	if (this->nextFreeFrameIndex >= NUMBER_OF_PAGEFRAMES) {
		void* kickedChunkAddr;
		if (!this->readQueue.isEmpty()) {
			kickedChunkAddr = this->readQueue.deQueue();
		}else{
			//kickout a chunk in the writeQueue
			kickedChunkAddr = this->writeQueue.deQueue();
			unsigned kickedPageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) pageStartAddress);
			while (mappingUnit.getPinnedBit(kickedPageFrameAddr) == PINNED) {
				kickedChunkAddr = this->writeQueue.deQueue();
				kickedPageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) pageStartAddress);
				this->writeQueue.enQueue(kickedChunkAddr);
			}
			this->pageOut(kickedChunkAddr);
		}
		mapOut(kickedChunkAddr);
	}

	cout << "where 6 0 6 0" << endl;
	//unmap page of logical memory
	munmap(pageStartAddress, PAGESIZE);
	cout << "where 6 0 6 1" << endl;
	//map page frame for PT 
	caddr_t addrFirstPT = (caddr_t) mmap(pageStartAddress, PAGESIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_FIXED, this -> fd, nextFreeFrameIndex*PAGESIZE);
    if(addrFirstPT == (caddr_t) MAP_FAILED)
    {
        cerr << "|###> Error: Mmap PT Failed" <<endl;
        exit(1);
    }
	nextFreeFrameIndex++;
	cout << "where 6 0 6 2" << endl;
	//book keeping for the activated page -> you have to do both steps
	
	this->writeQueue.enQueue(pageStartAddress);
	cout << "where 6 0 6 3" << endl;
}

void VirtualMem::fixPermissions(void* address)
{
	cout << "where 0" << endl;
	void* pageStartAddr = findStartAddress(address);
	cout << "get " << address << " find " << pageStartAddr << " where 1" << endl;
	unsigned* pagePTEntryAddr = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) address);

	unsigned phyAddr = ((char*) pageStartAddr) - ((char*) virtualMemStartAddress);
	unsigned first10Bits = mappingUnit.phyAddr2PDIndex(phyAddr);
	if (pagePTEntryAddr == 0) {
		addPTEntry2PD(virtualMemStartAddress + first10Bits);
		pagePTEntryAddr = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) address);
	}

    unsigned pageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) address);
	cout << "where 3 " << pageFrameAddr << endl;

	permission_change permissionChange;

	//determine which permission change is the right one
	if(mappingUnit.getPresentBit(pageFrameAddr) == NOT_PRESENT)
	{
		//this is the case when we change the permission from non to read
		if(this->nextFreeFrameIndex < NUMBER_OF_PAGEFRAMES)
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
	cout << "where 4 " << permissionChange << endl;


	switch(permissionChange)
	{
		case NONTOREAD_NOTFULL:
			cout << "where 5" << endl;
			//if there is already data on the disk, we have to this data in
			if (mappingUnit.getAccessed(pageFrameAddr) == ACCESSED) {
				this->pageIn(pageStartAddr);
			} else {
				mappingUnit.setAccessed(pagePTEntryAddr, ACCESSED);
			}
			cout << "where 6" << endl;
			mapIn(pageStartAddr);
			cout << "where 7" << endl;
			readPageActivate(pageStartAddr);
			cout << "where 8" << endl;
			break;

		case NONTOREAD_FULL:
			cout << "where 9" << endl;
			//first we have to kick out one chunk, preferibly in the readQueue
			void* kickedChunkAddr;
			if (!this->readQueue.isEmpty()) {
				kickedChunkAddr = this->readQueue.deQueue();
			}else{
				//kickout a chunk in the writeQueue
				kickedChunkAddr = this->writeQueue.deQueue();
    			unsigned kickedPageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) pageStartAddr);
				while (mappingUnit.getPinnedBit(kickedPageFrameAddr) == PINNED) {
					kickedChunkAddr = this->writeQueue.deQueue();
					kickedPageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) pageStartAddr);
					this->writeQueue.enQueue(kickedChunkAddr);
				}
				this->pageOut(kickedChunkAddr);
			}
			cout << "where 10" << endl;
			//now just deactivate all the stuff
			mapOut(kickedChunkAddr);

			//last but not least: activate the chunk just like in case 1
			if (mappingUnit.getPresentBit(pageFrameAddr) == NOT_PRESENT && mappingUnit.getPresentBit(pageFrameAddr) == ACCESSED) {
				this->pageIn(pageStartAddr);
			}
			mappingUnit.setAccessed(pagePTEntryAddr, ACCESSED);
			
			mapIn(pageStartAddr);
			cout << "where 11" << endl;
			readPageActivate(pageStartAddr);
			cout << "where 12" << endl;
			break;


		case READTOWRITE:
			cout << "where 13" << endl;
			//first we have to delete the chunk in the read queue
			this->readQueue.deQueue(pageStartAddr);
			//last we set all the flags right
			writePageActivate(pageStartAddr);
			break;
	}
}

void VirtualMem::readPageActivate(void* pageStartAddr)
{
	unsigned* pageTableEntry = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) pageStartAddr);
	
	mappingUnit.setReadAndWriteBit(pageTableEntry, READ);
	mprotect(pageStartAddr, PAGESIZE, PROT_READ);
	this->readQueue.enQueue(pageStartAddr);
}

void VirtualMem::writePageActivate(void* pageStartAddr)
{
	unsigned* pageTableEntry = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) pageStartAddr);
	
	mappingUnit.setReadAndWriteBit(pageTableEntry, WRITE);
	mprotect(pageStartAddr, PAGESIZE, PROT_WRITE);
	this->writeQueue.enQueue(pageStartAddr);
}

void VirtualMem::pageOut(void* kickedChunkAddr)
{
	off_t offset = reinterpret_cast<off_t>(kickedChunkAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileWrite(kickedChunkAddr, offset , PAGESIZE);

	unsigned* pageTableEntry = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) kickedChunkAddr);
    unsigned pageFrameAddr = *(pageTableEntry);

	pageoutPointer = pageFrameAddr;
}


void VirtualMem::pageIn(void* chunckStartAddr)
{
	off_t offset = reinterpret_cast<off_t>(chunckStartAddr)-reinterpret_cast<off_t>(this->virtualMemStartAddress); 
	this->swapFile.swapFileRead(chunckStartAddr, offset , PAGESIZE);
}

void VirtualMem::mapOut(void* pageStartAddress) {
	//map out shared memory file
	munmap(pageStartAddress, PAGESIZE);

	//map in MAP_Anonymous (simulation for no physical nemory behind it)
	mmap(pageStartAddress, PAGESIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

	unsigned* pageTableEntry = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) pageStartAddress);
	
	mappingUnit.setPresentBit(pageTableEntry, NOT_PRESENT);
}

void VirtualMem::mapIn(void* pageStartAddress) {
	cout << "where 6 0" << endl;
	addPageEntry2PT((unsigned*) pageStartAddress);
	cout << "where 6 1" << endl;

	//unmap the virtual space
	munmap(pageStartAddress, PAGESIZE);
	cout << "where 6 2" << endl;
	
	//translate logical to physical address
	unsigned pageFrameAddr = mappingUnit.logAddr2PF(virtualMemStartAddress, (unsigned*) pageStartAddress);
	unsigned* pageTableEntryAddr = mappingUnit.logAddr2PTEntryAddr(virtualMemStartAddress, (unsigned*) pageStartAddress);
	cout << "where 6 3" << endl;
	unsigned pageFrame = mappingUnit.phyAddr2page(pageFrameAddr);//without Offset
	cout << "where 6 4" << endl;

	//map in the physical space
	void* addr = mmap(pageStartAddress, PAGESIZE, PROT_NONE, MAP_PRIVATE | MAP_FIXED, this -> fd, pageFrame);
    if(addr == MAP_FAILED) {
    	cerr << "|###> Error: phy Mmap Failed from " << pageStartAddress << " to " << pageFrame << endl;
    	exit(1);
    }

	mappingUnit.setPresentBit(pageTableEntryAddr, PRESENT);
	//cout << addr << " mmap the same as " << pageStartAddress << endl;
	//exit(1);
}

void VirtualMem::addPageEntry2PT(unsigned* startAddrPage) {
	cout << "where 6 0 1" << endl;
	unsigned phyAddr = ((char*) startAddrPage) - ((char*) virtualMemStartAddress);
	//cout << "where 6 0 0" << endl;
	//unsigned first20Bits = mappingUnit.phyAddr2page(phyAddr);
	cout << "where 6 0 2 " << phyAddr << endl;
	unsigned first10Bits = mappingUnit.phyAddr2PDIndex(phyAddr);
	cout << "where 6 0 3 " << first10Bits << endl;
	unsigned second10Bits = mappingUnit.phyAddr2PTIndex(phyAddr);
	cout << "where 6 0 4 " << second10Bits << endl;
	unsigned pageTableAddr = *(virtualMemStartAddress + first10Bits);
	cout << pageTableAddr << " where 6 0 5 present? " << mappingUnit.getPresentBit(pageTableAddr) << endl;

	
	//if the PT not existing then create it by setting the presentBit
	if (mappingUnit.getPresentBit(pageTableAddr) == NOT_PRESENT) {
		addPTEntry2PD(virtualMemStartAddress + first10Bits);
	}
	cout << "where 6 0 9 " << mappingUnit.cutOfOffset(pageTableAddr) << endl;
	//add the page in PT
	*(virtualMemStartAddress + (mappingUnit.cutOfOffset(pageTableAddr)*PAGETABLE_SIZE) + second10Bits) = (nextFreeFrameIndex << 12) | mappingUnit.createOffset(1,0,1,0);
	cout << "where 6 0 10" << endl;
	nextFreeFrameIndex++;
}

void VirtualMem::addPTEntry2PD(unsigned* pdEntryOfPT) {
	initializePT(((char*) virtualMemStartAddress) + mappingUnit.phyAddr2page( *(pdEntryOfPT) ));
	//change PT to Present in PD
	mappingUnit.setPresentBit(pdEntryOfPT, PRESENT);
}

void* VirtualMem::findStartAddress(void* address)
{
	unsigned pageStart = mappingUnit.phyAddr2page(((char*) address) - ((char*) virtualMemStartAddress));
	return (void*) (((char*) virtualMemStartAddress) + pageStart);
   
   /*size_t address        =  reinterpret_cast<size_t>(chunckStartAddress); 
   size_t startAddress   =  reinterpret_cast<size_t>(this->virtualMemStartAddress); 
   size_t lastAddress    =  startAddress + PAGESIZE * NUMBER_OF_PAGES; 
   for(unsigned long chunkStartAddress = startAddress; chunkStartAddress < lastAddress; chunkStartAddress += PAGESIZE)
   {
        if(address >= chunkStartAddress && address < chunkStartAddress + PAGESIZE) 
            return (void*) chunkStartAddress; 
   }
   return NULL; */
}


void* VirtualMem::getStart()
{
    return (void*) (this->virtualMemStartAddress + ((NUMBER_OF_PT+1) * PAGETABLE_SIZE));
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

void VirtualMem::fillList(list<int>* virtualMem, list<unsigned>* physicalMem) {
	//virtualspace 
    unsigned* ptr1 = (unsigned*) virtualMemStartAddress;
    size_t i = 0;

    while (i < PAGETABLE_SIZE) {
        // 0 or 1 or 2 = none or read or write

		//searching in PD for present PT
		if (mappingUnit.getPresentBit(*(ptr1)) == 1) {
			//found one at ...

			//searching in PT for present Pages
			unsigned* ptrToPT = (virtualMemStartAddress + (mappingUnit.cutOfOffset(*(ptr1))*PAGETABLE_SIZE));
			for (int j = 0; j < PAGETABLE_SIZE; j++) {
				if (mappingUnit.getPresentBit(*(ptrToPT)) == 1) {
					//found one
					if (mappingUnit.getReadAndWriteBit(*(ptrToPT)) == 0) {
						virtualMem -> push_back(1);
						physicalMem -> push_back( mappingUnit.phyAddr2page(*(ptrToPT)) >> 12 );
					} else {
						virtualMem -> push_back(2);
						physicalMem -> push_back( mappingUnit.phyAddr2page(*(ptrToPT)) >> 12 ) ;
					}
						
				} else {
					virtualMem -> push_back(0);
				}
				ptrToPT++;
			}
		} else {
			for (int j = 0; j < PAGETABLE_SIZE; j++) {
				virtualMem -> push_back(0);
			}
		}

        ptr1++;
        i++;
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
	//need to be changed

	/*int k = writeQueue.size();
	int l = readQueue.size();
	for (int i = 0; i < k; i++) {
		void* kickedWriteChunkAddr = this->writeQueue.deQueue();
		kickedPageDeactivate(kickedWriteChunkAddr);
	}

	for (int i = 0; i < l; i++) {
		void* kickedReadChunkAddr = this->readQueue.deQueue();
		kickedPageDeactivate(kickedReadChunkAddr);
	}

	this -> pinnedPages = 0;*/
}
