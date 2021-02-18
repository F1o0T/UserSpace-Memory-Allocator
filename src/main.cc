#include <iostream>
#include <fstream>
#include "system/VirtualMem.h"
#include "runtime/FirstFitHeap.h"
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"
using namespace std; 

#define MALLOC


 //a simple version of bubble sort
void bubbleSort(unsigned* array, unsigned nrElements)
{
    for (unsigned i = 0; i < nrElements * 1024; i+= 1024) {
    	//cout << "Outer " << i << endl;  
        for (unsigned j = i; j < nrElements * 1024; j += 1024) {
        	//cout << "Inner " << i << endl;
            if (array[i] > array[j]) {
                unsigned temp = array[i];
                //cout << temp << endl; 
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}

// void* operator new[](size_t size, int dummy) {
//     cout << "new []" << endl; 
//     return FirstFitHeap::malloc(size);
// }


void* operator new(size_t size) {
    cout << "new" << endl; 
    cout << size << endl;
    return FirstFitHeap::malloc(size);
}

void operator delete(void* ptr) {
    FirstFitHeap::free(ptr);
}

void operator delete[](void* ptr) {
    FirstFitHeap::free(ptr);
}

int main(int argc, char** argv)
{
    ofstream myfile;
    FirstFitHeap heap;
    FirstFitHeap::initHeap();
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	cout << "dfsdfsdffsafsafdafdafdafasfffasdffafafafafdsfafdfsdf" << endl;
	CycleTimer t;
	namespace po = boost::program_options;
    //define available arguments
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help message")
        ("numbers,n", po::value<unsigned>()->required(), "How many numbers you want for the bubblesort list")
        (",r", po::value<uint64_t>()->default_value(10), "Repeat benchmark <arg> times")
        (",o", po::value<string>()->default_value("values.csv"),"Name of the output file");
        // ("pagesframes,p", po::value<unsigned>()->required(), "Number of pageframes available")
    //read arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    //did someone say 'help'?
    if (vm.count("help")) {
        std::cerr << desc << std::endl;
        return 0;
    }
    //read arguments into variables
    po::notify(vm);
    unsigned nrElements = vm["numbers"].as<unsigned>();
    uint64_t runs = vm["-r"].as<uint64_t>();
    string outputFileName = vm["-o"].as<string>();
    myfile.open(outputFileName, std::ofstream::trunc);

    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////

    #ifdef MALLOC
    unsigned *array = (unsigned*) FirstFitHeap::malloc(nrElements*4096);
    #else
    unsigned *array = new unsigned[nrElements*4096];
    #endif
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    for(unsigned i = 0; i < nrElements; i++)
    {	
    	array[i*1024] = nrElements-i; 
    }

    for(unsigned i = 0; i < nrElements; i++)
    {	
    	cout << array[i*1024] << endl;  
    }
    
    for(int i = 0; i < runs; i++)
    {
    	t.start();
    	bubbleSort(array, nrElements);
    	uint64_t time = t.stop();
    	if(i != runs -1)
    	{
    		myfile << time << ",";
    	}
    	else 
    	{
    		myfile << time;
    	}
    }
    myfile << "\n";
    for(unsigned i = 0; i < nrElements; i++)
    {	
    	cout << array[i*1024] << endl; 
    }
    cout << array << endl;
    FirstFitHeap::free(array);
    heap.destroyTimer();
    return 0;
}
