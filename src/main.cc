#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "gui/MemoryGUI.h"
#include "system/VirtualMem.h"

//size of a chunk
static const unsigned DEFAULT_CHUNKSIZE = 4096;

//VirtualMem to simulate logical and physical memory
VirtualMem vMem;

DrawingWindow* window;
MemoryGUI* gui;

bool showGUI = false;

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
            //cout << i << " = i and " << j << " = j" << endl;
            if (showGUI) {
				gui -> drawMemory();
				//sleep(1);
				////cout << "|>>> Write a char: "; char ch; 
				//cin >> ch; 
		    }
        }
    }
}



void signalHandeler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    if(info->si_code == SEGV_ACCERR)
    {   
        //cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        vMem.fixPermissions(info->si_addr);
        //cout << "|>>> Permissions were fixed!" << endl;
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        //cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }

} 

int main(int argc, char** argv)
{
    //cout << "main 1" << endl;
    vMem.initializeVirtualMem();
    
    if (showGUI) {
        window = new DrawingWindow(width, height, "GUI");
        gui = new MemoryGUI(&vMem, window, MO_PAGE);
        gui -> drawMemory();
    }
    //cout << "main 2" << endl;
    ///////////////////////////////////////////
    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandeler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
    ///////////////////////////////////////////
    //cout << "main 3" << endl;

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
    char* memStart = static_cast<char*>(vMem.getStart());
    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = (unsigned*) (memStart + (i * 4096));
    }
    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        //cout << "MEM start at = " << vMem.getStart() << endl;
        //cout << "address to access " << blocks[i] << endl;
        if (showGUI) {
            gui -> drawMemory();		
            //cout << "|>>> Write a char: "; char ch; 
		    //cin >> ch; 
        }
        //cout << "the i of the for loop = " << i << endl;
        *blocks[i] = refNumbers[i]; //pseudo-random values
        cout << "right? " << refNumbers[i] << " == " << *blocks[i] << endl;
        ////////////////
    }
    for (unsigned i = 0; i < nrElements; i++) {
        cout << "liste Element " << i << " = " << *blocks[i] << endl;
    }
    cout << "lol ok" << endl;
    bubbleSort(blocks, nrElements);

    if (showGUI) {
        cout << "|>>> Write a char to close: "; char ch; 
		cin >> ch;
        delete(window);
        delete(gui);
    }

    for (unsigned i = 0; i < nrElements; i++) {
        cout << "liste Element " << i << " = " << *blocks[i] << endl;
    }

    cout << "ok end of code" << endl;
    return 0;
}