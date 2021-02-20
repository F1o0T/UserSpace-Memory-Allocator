#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "memalloc.cc"
using namespace std; 
void bubbleSort(unsigned* array, unsigned nrElements);

int main(int argc, char** argv)
{  
    cout <<  "start Main" << endl;
    memAllocInitialize();
    cout << "#############################################" << endl;
    cout << "Beginning of important code" << endl;
    cout << "#############################################" << endl;
    int *array = (int*) malloc(10);   
    cout << (void*) array;
}

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