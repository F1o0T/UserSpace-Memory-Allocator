#ifndef AddressMapping_h
#define AddressMapping_h

#include <sys/types.h>
#include <iostream>
#include <sys/mman.h>

#define READ 0
#define WRITE 1
#define NOT_PRESENT 0
#define PRESENT 1
#define NOT_PINNED 0
#define PINNED 1
#define NOT_ACCESSED 0
#define ACCESSED 1
#define NO_LRU 0
#define LRU 1

using namespace std;

class AddressMapping {
public:
     unsigned logAddr2PF(unsigned* virtualMemStart, unsigned* logAddr);

     unsigned* logAddr2PTEntryAddr(unsigned* virtualMemStart, unsigned* logAddr);

     unsigned phyAddr2page(unsigned logaddr);

     unsigned phyAddr2PDIndex(unsigned page);

     unsigned phyAddr2PTIndex(unsigned page);

     unsigned cutOfOffset(unsigned logaddr);

     unsigned createOffset(bool presentBit, bool read_writeBit, bool accessed, bool pinnedBit, bool lruBit);

     unsigned getPresentBit(unsigned phyAddr);

     void setPresentBit(unsigned* tableEntry, bool presentBit);

     unsigned getReadAndWriteBit(unsigned phyAddr);

     void setReadAndWriteBit(unsigned* tableEntry, bool read_writeBit);

     unsigned getAccessed(unsigned phyAddr);

     void setAccessed(unsigned* tableEntry, bool accessed);

     unsigned getPinnedBit(unsigned phyAddr);

     void setPinnedBit(unsigned* tableEntry, bool pinnedBit);

     unsigned getLruBit(unsigned phyAddr);

     void setLruBit(unsigned* tableEntry, bool lruBit);

     int fd;
};

#endif