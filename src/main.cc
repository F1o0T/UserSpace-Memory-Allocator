#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"
#include <string>

#include "gui/MemoryGUI.h"
#include "system/VirtualMem.h"

//size of a chunk
static const unsigned DEFAULT_CHUNKSIZE = 4096;

//VirtualMem to simulate logical and physical memory
VirtualMem vMem;

DrawingWindow* window;
MemoryGUI* gui;

bool showGUI = true;

//a simple version of bubble sort
void bubbleSort(unsigned** array, unsigned nrElements)
{
    for (unsigned i = 0; i < nrElements; i++) {
        for (unsigned j = i; j < nrElements; j++) {
            if (*(array[i]) > *(array[j])) {
                int temp = *(array[i]);
                *(array[i]) = *(array[j]);
                *(array[j]) = temp;
            }
            if (showGUI) {
				gui -> drawMemory();
				sleep(1);
				//cout << "|>>> Write a char: "; char ch; 
				//cin >> ch; 
		    }
        }
    }
}



void signalHandeler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    if(info->si_code == SEGV_ACCERR)
    {   
        cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        vMem.fixPermissions(info->si_addr);
        cout << "|>>> Permissions were fixed!" << endl;
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }

} 

int main(int argc, char** argv)
{
    bool writeBackAll = false;
    cout << "main 1" << endl;
    vMem.initializeVirtualMem(writeBackAll);
    
    if (showGUI) {
        window = new DrawingWindow(width, height, "GUI");
        gui = new MemoryGUI(&vMem, window, MO_PAGE);
    }
    cout << "main 2" << endl;
    gui -> drawMemory();
    ///////////////////////////////////////////
    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandeler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
    ///////////////////////////////////////////
    cout << "main 3" << endl;

    //each element has a size of blockSize
    //how many can we store in the memory?
    unsigned nrElements = 20;
    //this array stores pointers to the blocks
    unsigned* blocks[nrElements];
    unsigned refNumbers[nrElements];
    for (unsigned i = 0; i < nrElements; i++) {
        refNumbers[i] = nrElements-i; //reverse sorted
    }

    //store the start address of each block in the array
    unsigned* memStart = static_cast<unsigned*>(vMem.getStart());
    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = memStart + (i * 1024);
    }
    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        cout << "MEM start at = " << vMem.getStart() << endl;
        cout << "address to access " << blocks[i] << endl;
        *blocks[i] = refNumbers[i]; //pseudo-random values
    }
    cout << "lol ok" << endl;
    bubbleSort(blocks, nrElements);

    if (showGUI) {
        cout << "|>>> Write a char to close: "; char ch; 
		cin >> ch;
        delete(window);
        delete(gui);
    }

    cout << "ok end of code" << endl;
    return 0;
}