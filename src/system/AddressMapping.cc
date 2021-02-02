#include "system/AddressMapping.h"


//returns the corresponding physical address of a logical address
unsigned AddressMapping::log2phys(caddr_t logaddr) {
	unsigned offset = addr2offset(logaddr);
	unsigned page = addr2page(logaddr); 
	unsigned pageFrame = page2frame(page);
	unsigned physicalAddress = pageFrame + offset; 
	return physicalAddress; 
}

//returns page identifaction part of an address, e.g.: 0000000000 1010101010 000000000000
unsigned AddressMapping::addr2page(caddr_t logaddr) {
    //turn into number so we can do our math with it
    size_t addressAsNr = reinterpret_cast<size_t> (logaddr);
    //shift until page part of the number is right
    addressAsNr = addressAsNr >> 12;
    //make all but the last 10 Bits zero
    addressAsNr = addressAsNr & 0x3FF;
    //shift the number back to the left
    addressAsNr = addressAsNr  << 12;
    //cast that should be not a problem
    unsigned page = (unsigned) addressAsNr;

	return page; 
}

//returns the offset part of an address
unsigned AddressMapping::addr2offset(caddr_t logaddr) {
    //turn into number so we can do out math on it
    size_t addressAsNr = reinterpret_cast<size_t> (logaddr);
    //make all but the last 12 Bits zero
    addressAsNr = addressAsNr & 0xFFF;
    //cast that should be not a problem
    unsigned offset = (unsigned) addressAsNr;
	return offset; 
}

//returns the physical frame of the given page 
unsigned AddressMapping::page2frame(unsigned page) {
    /*
	caddr_t *pageTableAddress = *(pageDirectoryAddress + page2pageDirectoryIndex(page)); 
	caddr_t *pageFrameAddress = *(pageTableAddress + page2pageTableIndex(page)); 
	unsigned pageFrame = (unsigned)(pageFrameAddress) >> 12;
    */
}

//returns the index for the pagetable 
unsigned AddressMapping::page2pageTableIndex(unsigned page) {
	unsigned pageTableIndex = page & 0x3FF;
	return pageTableIndex; 
}

//returns the index for the pagedirectory
unsigned AddressMapping::page2pageDirectoryIndex(unsigned page) {
    unsigned pageDirectoryIndex = page >> 10;
}
