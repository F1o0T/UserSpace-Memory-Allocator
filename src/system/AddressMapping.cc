#include "system/AddressMapping.h"

/*//IMPORTANT: the adresses have to be the differnce between the logical address and the beginning if the mapping!!!


//returns the corresponding physical address of a logical address which is reprensented by an 32 Bit Unsigned
unsigned AddressMapping::log2phys(unsigned *virtualMemStart, unsigned* logaddr) {
	unsigned offset = addr2offset(logaddr);
	unsigned page = addr2page(logaddr); 
	unsigned pageFrame = page2frame(virtualMemStart, page);
	unsigned physicalAddress = pageFrame + offset; 
	return physicalAddress;
}

//returns page identifaction part of an address, e.g.: 1010101010 1010101010 0000000000
unsigned AddressMapping::addr2page(unsigned *logaddr) {
    //turn into number so we can do our math with it
    size_t addressAsNr = reinterpret_cast<size_t> (logaddr);
    //shift until page part of the number is right
    logaddr = logaddr >> 12;
    //shift the number back to the left
    logaddr = logaddr << 12;

	return logaddr;
}

//returns the offset part of an address
unsigned AddressMapping::addr2offset(unsigned *logaddr) {
    //turn into number so we can do out math on it
    size_t addressAsNr = reinterpret_cast<size_t> (logaddr);
    //make all but the last 12 Bits zero
	return logaddr & 0xFFF;
}

//returns the physical frame of the given page 
unsigned AddressMapping::page2frame(unsigned *virtualMemStart ,unsigned page) {
    unsigned distanceToPTinlogMem = *(virtualMemStart + page2pageDirectoryIndex(page));
    unsigned pageFrame = *(virtualMemStart + distanceToPTinlogMem + page2pageTableIndex(page));
    return pageFrame;
}

//returns the index for the pagetable, e.g.: 1100000011 0000110000 12*0 -> 22*0 1100000011
unsigned AddressMapping::page2pageTableIndex(unsigned page) {
    unsigned pageTableIndex = page >> 12;
    pageTableIndex = page & 0x3FF;//to make sure the other 22 Bits are 0
	return pageTableIndex;
}

//returns the index for the pagedirectory e.g.: 
unsigned AddressMapping::page2pageDirectoryIndex(unsigned page) {
    unsigned pageDirectoryIndex = page >> 22;
    return pageDirectoryIndex;
}*/

/////////////////////////////////////////////////////////////////////////

unsigned AddressMapping::logAddr2PF(unsigned* virtualMemStart, unsigned* logAddr) {
    unsigned phyAddr = ((char*) logAddr) - ((char*) virtualMemStart);
    unsigned addrOfPT = *(virtualMemStart + phyAddr2PDIndex(phyAddr));
    unsigned addrOfPF = *(virtualMemStart + cutOfOffset(addrOfPT) + phyAddr2PTIndex(phyAddr));
    return addrOfPF; 
}

unsigned* AddressMapping::logAddr2PTEntry(unsigned* virtualMemStart, unsigned* logAddr) {
    unsigned phyAddr = ((char*) logAddr) - ((char*) virtualMemStart);
    cout << "log2PT 1 " << phyAddr << endl;
    unsigned addrOfPT = *(virtualMemStart + phyAddr2PDIndex(phyAddr));
    cout << "log2PT 2 " << addrOfPT << endl;
    unsigned* pageTableEntry = virtualMemStart + cutOfOffset(addrOfPT) + phyAddr2PTIndex(phyAddr);
    cout << "log2PT 3 " << pageTableEntry << " mit " << cutOfOffset(addrOfPT) << " und " << phyAddr2PTIndex(phyAddr) << endl;
    return pageTableEntry;
}

// 1111111111 1111111111 000000000000
unsigned AddressMapping::phyAddr2page(unsigned logaddr) {
    logaddr = logaddr >> 12;
	return logaddr << 12;
}

unsigned AddressMapping::phyAddr2PDIndex(unsigned page) {
	return page >> 22;
}

unsigned AddressMapping::phyAddr2PTIndex(unsigned page) {
    page = page << 10;
    return page >> 22;
}

unsigned AddressMapping::cutOfOffset(unsigned logaddr) {
	return logaddr >> 12;
}

/**
 * create and return an Offset build from parameters
 *
 * @param presentBit 0 = not present, 1 = present
 * @param read_writeBit 0 = read, 1 = write
 * @param pinnedBit 0 = not pinned, 1 = pinned
 * @param dirtyBit 0 = means swapfile = ram, 1 = means ram has changed data
 * @return complett Offset (you can add it to the address)
 */
unsigned AddressMapping::createOffset(bool presentBit, bool read_writeBit, bool pinnedBit, bool dirtyBit) {
    unsigned offset = 0;

    if (presentBit) {
        offset = offset | 0b1;
    }

    if (read_writeBit) {
        offset = offset | 0b10;
    }

    if (pinnedBit) {
        offset = offset | 0b1000;
    }

    if (dirtyBit) {
        offset = offset | 0b1000000;
    }

    return offset;
}

/**
 * @return 0 = not present, 1 = present
 */
unsigned AddressMapping::getPresentBit(unsigned phyAddr) {
    return phyAddr & 0b1;
}

unsigned AddressMapping::setPresentBit(unsigned phyAddr, bool presentBit) {
    if(presentBit == 1) {
        return phyAddr | 0b1;
    } else {
        return phyAddr & 0xFFFFFFFE;
    }
}

unsigned AddressMapping::getReadAndWriteBit(unsigned phyAddr) {
    return phyAddr & 0b10;
}

unsigned AddressMapping::setReadAndWriteBit(unsigned phyAddr, bool read_writeBit) {
    if(read_writeBit == 1) {
        return phyAddr | 0b10;
    } else {
        return phyAddr & 0xFFFFFFFE;
    }
}

unsigned AddressMapping::getPinnedBit(unsigned phyAddr) {
    return phyAddr & 0b10;
}

unsigned AddressMapping::setPinnedBit(unsigned phyAddr, bool pinnedBit) {
    if(pinnedBit == 1) {
        return phyAddr | 0b1000;
    } else {
        return phyAddr & 0xFFFFFFFE;
    }
}

unsigned AddressMapping::getDirtyBit(unsigned phyAddr) {
    return phyAddr & 0b10;
}

unsigned AddressMapping::setDirtyBit(unsigned phyAddr, bool dirtyBit) {
    if(dirtyBit == 1) {
        return phyAddr | 0b1000000;
    } else {
        return phyAddr & 0xFFFFFFFE;
    }
}