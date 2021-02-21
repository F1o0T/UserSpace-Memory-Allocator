#include "runtime/MemAlloc.h"
//#include <boost/program_options.hpp>

int main(int argc, char** argv)
{
    cout << "hwllo richard " <<endl; 
    void* array = malloc(10);
    cout << &array << endl;
}