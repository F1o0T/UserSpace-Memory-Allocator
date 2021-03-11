#include <iostream>

#include "system/VirtualMem.h"
//using namespace std;
//extern FirstFitHeap heap;
extern VirtualMem vMem;

int mallocNumber= 100;

int main(int argc, char** argv)
{
    cout << "pagesinRam:" << vMem.pagesinRAM << endl;
    
    std::cout << "start main" << std::endl;
    unsigned nrElements = 20;
    unsigned* blocks[nrElements];

    void* supArray[mallocNumber];

    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = (unsigned*) malloc(sizeof(unsigned));;
        //blocks[i] = malloc(sizeof(unsigned));
    }

    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        *blocks[i] = i;
    }


    for(int i = 0; i< mallocNumber; i++){
        supArray[i] = malloc(4096);
    //cout << "loop:" + i;
    cout << i << " pagesinRam:" <<  vMem.pagesinRAM << endl;

    }

    
    

    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[18] << std::endl;
    std::cout << "what is inside of " << *blocks[19] << std::endl;
    std::cout << "what is inside of " << *blocks[5] << std::endl;
    std::cout << "what is address of 0 = " << (void*) blocks[0] << std::endl;
    std::cout << "what is address of 5 = " << (void*) blocks[5] << std::endl;
    free(blocks[5]);
    std::cout << *((blocks[5]) -1) << std::endl;
    std::cout << "what is inside of " << *blocks[8] << std::endl;
    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[0] << std::endl;


for(int i = 0; i < mallocNumber; i++){
    free(supArray[i]);
    cout << i << " freed" << endl;

}

}