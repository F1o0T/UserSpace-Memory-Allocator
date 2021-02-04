#ifndef AddressMapping_h
#define AddressMapping_h

#include <sys/types.h>



class AddressMapping {
public:
     //returns the corresponding physical address of a logical address
     unsigned log2phys(unsigned *virtualMemStart, unsigned logaddr);

     //returns pageidentifaction part of an address
     unsigned addr2page(unsigned logaddr);
  
     //returns the offset part of an address
     unsigned addr2offset(unsigned logaddr);
  
     //returns the physical frame of the given page
     unsigned page2frame(unsigned *virtualMemStart, unsigned page);
  
     //returns the index for the pagetable
     unsigned page2pageTableIndex(unsigned page);
  
     //returns the index for the pagedirectory
     unsigned page2pageDirectoryIndex(unsigned page);

     unsigned createOffset(bool presentBit, bool read_writeBit, bool pinnedBit, bool accessBit, bool dirtyBit, bool pageSizeBit);

     unsigned getPresentBit(unsigned phyAddr);

     unsigned setPresentBit(unsigned phyAddr);

     unsigned getReadAndWriteBit(unsigned phyAddr);
};

#endif