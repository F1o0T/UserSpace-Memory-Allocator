#include <iostream>

//using namespace std;
//extern FirstFitHeap heap;

int main(int argc, char** argv)
{
    
    std::cout << "start main" << std::endl;
    unsigned nrElements = 20;
    unsigned* blocks[nrElements];

    
    
    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = (unsigned*) malloc(sizeof(unsigned));;
        //blocks[i] = malloc(sizeof(unsigned));
    }

    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        *blocks[i] = i;
    }

    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[18] << std::endl;
    std::cout << "what is inside of " << *blocks[19] << std::endl;
    std::cout << "what is inside of " << *blocks[5] << std::endl;
    std::cout << "what is address of 0 = " << (void*) blocks[0] << std::endl;
    std::cout << "what is address of 5 = " << (void*) blocks[5] << std::endl;
    free(blocks[5]);
    std::cout << "what is inside of " << *blocks[8] << std::endl;
    std::cout << "what is inside of " << *blocks[1] << std::endl;
    std::cout << "what is inside of " << *blocks[0] << std::endl;

}