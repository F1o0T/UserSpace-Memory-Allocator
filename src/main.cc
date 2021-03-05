#include "runtime/Memalloc.h"
//#include <boost/program_options.hpp>

//using namespace std;
extern FirstFitHeap heap;

int main(int argc, char** argv)
{
    
    cout << "start main" << endl;
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

    cout << "what is inside of " << *blocks[1] << endl;
    cout << "what is inside of " << *blocks[18] << endl;
    cout << "what is inside of " << *blocks[19] << endl;
    cout << "what is inside of " << *blocks[5] << endl;
    cout << "what is inside of " << *blocks[8] << endl;
    cout << "what is inside of " << *blocks[1] << endl;
    cout << "what is inside of " << *blocks[0] << endl;


}