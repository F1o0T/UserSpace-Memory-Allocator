#ifndef AddressMapping_h
#define AddressMapping_h

#include <sys/types.h>

class AddressMapping {
public:
     //returns the corresponding physical address of a logical address
     caddr_t log2phys(caddr_t logaddr);

     //returns pageidentifaction part of an address
     unsigned addr2page(caddr_t logaddr);
  
     //returns the offset part of an address
     unsigned addr2offset(caddr_t logaddr);
  
     //returns the physical frame of the given page
     unsigned page2frame(unsigned page);
  
     //returns the index for the pagetable
     unsigned page2pageTableIndex(unsigned page);
  
     //returns the index for the pagedirectory
     unsigned page2pageDirectoryIndex(unsigned page);
};

#endif