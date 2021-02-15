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

using namespace std;

class AddressMapping {
public:
     /*//returns the corresponding physical address of a logical address
     unsigned log2phys(unsigned *virtualMemStart, unsigned* logaddr);

     //returns pageidentifaction part of an address
     unsigned addr2page(unsigned* logaddr);
  
     //returns the offset part of an address
     unsigned addr2offset(unsigned* logaddr);
  
     //returns the physical frame of the given page
     unsigned page2frame(unsigned *virtualMemStart, unsigned page);
  
     //returns the index for the pagetable
     unsigned page2pageTableIndex(unsigned page);
  
     //returns the index for the pagedirectory
     unsigned page2pageDirectoryIndex(unsigned page);*/

     ///////////////////////////////////////////////////777

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

     void setLruBit(unsigned* tableEntry, bool lruBit);

     unsigned getLruBit(unsigned phyAddr);
};

#endif