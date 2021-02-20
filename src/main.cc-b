#include <iostream>
#include <fstream>
#include "system/VirtualMem.h"
#include "runtime/FirstFitHeap.h"
//#include "system/Memalloc.h"
//#include <boost/program_options.hpp>

using namespace std; 

// void bubbleSort(unsigned* array, unsigned nrElements)
// {
//     for (unsigned i = 0; i < nrElements * 1024; i+= 1024) {
//         for (unsigned j = i; j < nrElements * 1024; j += 1024) {
//             if (array[i] > array[j]) {
//                 unsigned temp = array[i];
//                 array[i] = array[j];
//                 array[j] = temp;
//             }
//         }
//     }
// }

void* operator new(size_t size) {
    //cout << "new is called with size = " << size << endl; 
    return FirstFitHeap::malloc(size);
}

void* operator new[](size_t size, int dummy) {
    //cout << "new []" << endl; 
    return FirstFitHeap::malloc(size);
}

void operator delete(void* ptr) {
    FirstFitHeap::free(ptr);
}

void operator delete[](void* ptr) {
    FirstFitHeap::free(ptr);
}

void free(void *ptr)
{
    FirstFitHeap::free(ptr); 
}

int main(int argc, char** argv)
{  
    FirstFitHeap heap;
    //FirstFitHeap::initHeap();
    cout <<  "start Main" << endl;
    //cout << "#############################################" << endl;
    //cout << "Beginning of important code" << endl;
    //cout << "#############################################" << endl;
    void* array = FirstFitHeap::malloc(10);   
    cout << array << endl;
}

































    // cout << "#############################################" << endl;
    // cout << "Beginning of Main" << endl;
    // cout << "#############################################" << endl; 
	// //////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////
	// namespace po = boost::program_options;
    // //define available arguments
    // po::options_description desc("Options");
    // desc.add_options()
    //     ("help,h", "Print help message")
    //     ("numbers,n", po::value<unsigned>()->required(), "How many numbers you want for the bubblesort list")
    //     (",r", po::value<uint64_t>()->default_value(10), "Repeat benchmark <arg> times")
    //     (",o", po::value<string>()->default_value("values.csv"),"Name of the output file");
    //     // ("pagesframes,p", po::value<unsigned>()->required(), "Number of pageframes available")
    // //read arguments
    // po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    // //did someone say 'help'?
    // if (vm.count("help")) {
    //     std::cerr << desc << std::endl;
    //     return 0;
    // }
    // //read arguments into variables
    // po::notify(vm);
    // unsigned nrElements = vm["numbers"].as<unsigned>();
    // uint64_t runs = vm["-r"].as<uint64_t>();
    // //////////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////