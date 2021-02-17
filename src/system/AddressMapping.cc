#include "system/AddressMapping.h"


unsigned AddressMapping::logAddr2PF(unsigned* virtualMemStart, unsigned* logAddr) {
    unsigned phyAddr = ((char*) logAddr) - ((char*) virtualMemStart);
    unsigned addrOfPT = *(virtualMemStart + phyAddr2PDIndex(phyAddr));
    char* pageTableEntry = ((char*) virtualMemStart) + phyAddr2page(addrOfPT) + (phyAddr2PTIndex(phyAddr)*4);
    return *((unsigned*) pageTableEntry);
}

unsigned* AddressMapping::logAddr2PTEntryAddr(unsigned* virtualMemStart, unsigned* logAddr) {
    unsigned phyAddr = ((char*) logAddr) - ((char*) virtualMemStart);
    unsigned addrOfPT = *(virtualMemStart + phyAddr2PDIndex(phyAddr));
    if (getPresentBit(addrOfPT) == NOT_PRESENT) {
        return 0;
    }
    char* pageTableEntry = ((char*) virtualMemStart) + phyAddr2page(addrOfPT) + (phyAddr2PTIndex(phyAddr)*4);
    return (unsigned*) pageTableEntry;
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
 * @param accessed 0 = not accessed yet, 1 = accessed it first time
 * @param pinnedBit 0 = not pinned, 1 = pinned
 * @return complett Offset (you can add it to the address)
 */
unsigned AddressMapping::createOffset(bool presentBit, bool read_writeBit, bool accessed, bool pinnedBit, bool lruBit) {
    unsigned offset = 0;

    if (presentBit) {
        offset = offset | 0b1;
    }

    if (read_writeBit) {
        offset = offset | 0b10;
    }

    if (accessed) {
        offset = offset | 0b100;
    }

    if (pinnedBit) {
        offset = offset | 0b1000;
    }

    if (lruBit) {
        offset = offset | 0b10000; 
    }
    return offset;
}

/**
 * @return 0 = not present, 1 = present
 */
unsigned AddressMapping::getPresentBit(unsigned phyAddr) {
    return phyAddr & 0b1;
}

void AddressMapping::setPresentBit(unsigned* tableEntry, bool presentBit) {
    if (presentBit == 1) {
        *(tableEntry) = *(tableEntry) | 0b1;
    } else {
        *(tableEntry) = *(tableEntry) & 0xFFFFFFFE;
    }
}

unsigned AddressMapping::getReadAndWriteBit(unsigned phyAddr) {
    return (phyAddr & 0b10) >> 1;
}

void AddressMapping::setReadAndWriteBit(unsigned* tableEntry, bool read_writeBit) {
    if (read_writeBit == 1) {
        *(tableEntry) = *(tableEntry) | 0b10;
    } else {
        *(tableEntry) = *(tableEntry) & 0xFFFFFFFD;
    }
}

unsigned AddressMapping::getAccessed(unsigned phyAddr) {
    return (phyAddr & 0b100) >> 2;
}

void AddressMapping::setAccessed(unsigned* tableEntry, bool accessed) {
    if (accessed == 1) {
        *(tableEntry) = *(tableEntry) | 0b100;
    } else {
        *(tableEntry) = *(tableEntry) & 0xFFFFFFFB;
    }
}

unsigned AddressMapping::getPinnedBit(unsigned phyAddr) {
    return (phyAddr & 0b1000) >> 3;
}

void AddressMapping::setPinnedBit(unsigned* tableEntry, bool pinnedBit) {
    if(pinnedBit == 1) {
        *(tableEntry) = *(tableEntry) | 0b1000;
    } else {
        *(tableEntry) = *(tableEntry) & 0xFFFFFFF7;
    }
}

unsigned AddressMapping::getLruBit(unsigned phyAddr) {
    return (phyAddr & 0b10000) >> 4;
}

void AddressMapping::setLruBit(unsigned* tableEntry, bool lruBit) {
    if(lruBit == 1) {
        *(tableEntry) = *(tableEntry) | 0b10000; 
    } else {
        *(tableEntry) = *(tableEntry) & 0xFFFFFEF;
    }
}