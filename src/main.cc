#include "runtime/MemAlloc.h"
//#include <boost/program_options.hpp>
using namespace std;

int main(int argc, char** argv)
{
    cout << "################### start Main ######################" << endl;
    unsigned nrElements = 20;
    unsigned* blocks[nrElements];
    void* array = malloc(10);
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
    cout << "################### end of Code ######################" << endl;
}