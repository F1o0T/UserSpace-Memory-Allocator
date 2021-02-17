#include <iostream>
#include "system/VirtualMem.h"
#include "runtime/FirstFitHeap.h"
#include <boost/program_options.hpp>
 
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
int main(int argc, char** argv)
{
	namespace po = boost::program_options;
    //define available arguments
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help message")
        ("numbers,n", po::value<unsigned>()->required(), "How many numbers you want for the bubblesort list");
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
    ///////////////
    FirstFitHeap heap;
    FirstFitHeap::initHeap();
    /////////////////
    // User Code
    unsigned *array = (unsigned*) FirstFitHeap::malloc(nrElements*4096);

    for(unsigned i = 0; i < nrElements; i++)
    {	
    	array[i*1024] = nrElements-i; 
    }
    cout << "Before Sorting" << endl; 
    for(unsigned i = 0; i < nrElements; i++)
    {	
    	cout << array[i*1024] << endl;  
    }

    bubbleSort(array, nrElements);
	cout << "After Sorting" << endl; 
    for(unsigned i = 0; i < nrElements; i++)
    {	
    	cout << array[i*1024] << endl;  
    }
    // array[0]    = 'A';
    // array[3500] = 'B';

    return 0;
}

































// DrawingWindow* window;
// MemoryGUI* gui;
// bool showGUI = false;










    // if (showGUI) {
    //     window = new DrawingWindow(width, height, "GUI");
    //     gui = new MemoryGUI(&vMem, window, MO_PAGE, nrElements);
    //     gui -> drawMemory();
    // }
 
    
    

    // if (showGUI) {
    //     cout << "|>>> Write a char to close: "; char ch; 
	// 	cin >> ch;
    //     delete(window);
    //     delete(gui);
    // }
    
    // cout << "ok end of code" << endl;


// namespace po = boost::program_options;
    // //define available arguments
    // po::options_description desc("Options");
    // desc.add_options()
    //     ("help,h", "Print help message")
    //     ("numbers,n", po::value<unsigned>()->required(), "How many numbers you want for the bubblesort list")
    //     ("pagesframes,p", po::value<unsigned>()->required(), "Number of pageframes available");

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
    // unsigned numberOfPF = vm["pagesframes"].as<unsigned>();