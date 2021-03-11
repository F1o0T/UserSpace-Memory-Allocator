#include <iostream>

#include "system/VirtualMem.h"
//using namespace std;
//extern FirstFitHeap heap;
extern VirtualMem vMem;

int mallocNumber= 100;

int main(int argc, char** argv)
{
    void* lol = malloc(4096);
    *((unsigned*) lol) = 300;
    unsigned lol2 = vMem.pagesinRAM;
    cout << "pagesinRam:" << lol2 << endl;
    cout << "pagesinRam:" << vMem.pagesinRAM << endl;
    
    std::cout << "start main" << std::endl;
    unsigned nrElements = 20;
    unsigned* blocks[nrElements];

    unsigned* supArray[mallocNumber];

    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = (unsigned*) malloc(sizeof(unsigned)*400);;
        //blocks[i] = malloc(sizeof(unsigned));
    }

    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        *blocks[i] = i;
    }
    cout << " pagesinRam:" <<  vMem.pagesinRAM << endl;
    std::cout << "what is address of 19 = " << (void*) blocks[19] << std::endl;

    blocks[nrElements-1] = (unsigned*) realloc(blocks[nrElements-1], 2000);


    for(int i = 0; i< mallocNumber; i++){
        supArray[i] = (unsigned*) malloc(4000);
        supArray[i] = (unsigned*) realloc(supArray[i], 4096);
        cout << i << " pagesinRam:" <<  vMem.pagesinRAM << endl;
    }

    
    

    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[18] << std::endl;
    std::cout << "what is inside of " << *blocks[19] << std::endl;
    std::cout << "what is inside of " << *blocks[5] << std::endl;
    std::cout << "what is address of 0 = " << (void*) blocks[0] << std::endl;
    std::cout << "what is address of 19 = " << (void*) blocks[19] << std::endl;
    free(blocks[5]);
    std::cout << *((blocks[5]) -1) << std::endl;
    std::cout << "what is inside of " << *blocks[8] << std::endl;
    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[0] << std::endl;
    std::cout << "what is inside of " << *supArray[99] << std::endl;
    std::cout << "what is inside of " << *supArray[98] << std::endl;
    std::cout << "what is inside of " << *supArray[3] << std::endl;


    for(int i = 0; i < mallocNumber; i++){
        free(supArray[i]);
        cout << i << " freed" << endl;

    }

}