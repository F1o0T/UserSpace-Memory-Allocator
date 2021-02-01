#include "system/AddressMapping.h"

//returns the corresponding physical address of a logical address
caddr_t AddressMapping::log2phys(caddr_t logaddr) {

}

//returns pageidentifaction part of an address
unsigned AddressMapping::addr2page(caddr_t logaddr) {

}

//returns the offset part of an address
unsigned AddressMapping::addr2offset(caddr_t logaddr) {

}

//returns the physical frame of the given page 
unsigned AddressMapping::page2frame(unsigned page) {

}

//returns the index for the pagetable 
unsigned AddressMapping::page2pageTableIndex(unsigned page) {

}

//returns the index for the pagedirectory
unsigned AddressMapping::page2pageDirectoryIndex(unsigned page) {
    
}