#include "runtime/Memalloc.h"
//#include <boost/program_options.hpp>

//using namespace std;
extern FirstFitHeap heap;

int main(int argc, char** argv)
{
    cout << "start main" << endl;
    unsigned nrElements = 20;
    unsigned* blocks[nrElements];

    void *array1 = malloc(1024);
    
    void* array = new int[30];
    cout << "arraysize" << sizeof(array) << endl;
    unsigned* memStart = static_cast<unsigned*>(array);
    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = memStart + (i * 1024);
    }

    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        *blocks[i] = i;
    }

    cout << "what is inside of " << *blocks[1] << endl;
    cout << "what is inside of " << *blocks[18] << endl;
    cout << "what is inside of " << *blocks[19] << endl;
    cout << "what is inside of " << *blocks[5] << endl;
    cout << "what is inside of " << *blocks[8] << endl;
    cout << "what is inside of " << *blocks[1] << endl;
    cout << "what is inside of " << *blocks[0] << endl;

}