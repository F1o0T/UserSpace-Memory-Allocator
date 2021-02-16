#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <boost/program_options.hpp>
#include "system/VirtualMem.h"
#include "runtime/FirstFitHeap.h"
#include "gui/MemoryGUI.h"


VirtualMem vMem;
void VirtualMem::signalHandler(int sigNUmber, siginfo_t *info, void *ucontext)
{
	if(info->si_code == SEGV_ACCERR)
    {   
        //cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        vMem.fixPermissions(info->si_addr);
        //cout << "|>>> Permissions were fixed!" << endl;
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }
}

int main(int argc, char** argv)
{
    ///////////////////////////////////////////////
	// SignalHandling
    struct sigaction SigAction;
	SigAction.sa_sigaction = vMem.signalHandler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
	///////////////////////////////////////////////
    ////////////////
    
    FirstFitHeap heap(vMem);
    heap.initHeap();
    /////////////////

    // User Code
    char *array = (char*) heap.alloc(4100);   //malloc()
    array[0]    = 'A';
    array[4099] = 'B';

    return 0;
}

































// DrawingWindow* window;
// MemoryGUI* gui;
// bool showGUI = false;




// //a simple version of bubble sort
// void bubbleSort(unsigned** array, unsigned nrElements)
// {
//     for (unsigned i = 0; i < nrElements; i++) {
//         for (unsigned j = i; j < nrElements; j++) {
//             if (*(array[i]) > *(array[j])) {
//                 // cout << "=============================================" << endl;
//                 // cout << "AccessStack Current Elements " << endl; 
//                 // cout << "=============================================" << endl;  
//                 // vMem.accessStack.display();
//                 // cout << "=============================================" << endl; 
//                 int temp = *(array[i]);
//                 *(array[i]) = *(array[j]);
//                 *(array[j]) = temp;
//                 cout << "Sorting " << endl; 
//                 // unsigned test0 = *(array[0]); 
//                 // unsigned test1 = *(array[1]);
//                 // unsigned test2 = *(array[2]);
//                 // cout << "In bubble sort: " << test0 << " is @ " << (void*) array[0] << endl; 
//                 // cout << "In bubble sort: " << test1 << " is @ " << (void*) array[1] << endl; 
//                 // cout << "In bubble sort: " << test2 << " is @ " << (void*) array[2] << endl; 
//             }

//             if (showGUI) {
// 				gui -> drawMemory();
// 				sleep(1);
// 				////cout << "|>>> Write a char: "; char ch; 
// 				//cin >> ch; 
// 		    }
//         }
//     }
// }





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