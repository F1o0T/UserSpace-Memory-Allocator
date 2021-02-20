#include <iostream>
#include <fstream>
#include "system/VirtualMem.h"
//#include "runtime/FirstFitHeap.h"
//#include "system/Memalloc.h"
//#include <boost/program_options.hpp>

using namespace std; 

VirtualMem vMem;

void signalHandeler(int sigNUmber, siginfo_t *info, void *ucontext) {

	vMem.stopTimer();

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

	vMem.startTimer();

    //wait on ProtNoneAll
    while(true){
        if (vMem.protNoneAllFlag == false) {
            break;
        }
        cout << "ok lets wait for protNone" << endl;
    }
} 

int main(int argc, char** argv)
{
    cout << "################### start Main ######################" << endl;

    ///////////////////////////////////////////
    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandeler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
    ///////////////////////////////////////////

    unsigned nrElements = 20;
    unsigned* blocks[nrElements];
    
    unsigned* memStart = static_cast<unsigned*>(vMem.getStart());
    for (unsigned i = 0; i < nrElements; i++) {
        blocks[i] = memStart + (i * 1024);
    }

    //initialize the field
    for (unsigned i = 0; i < nrElements; i++) {
        if (i == 10) {
            vMem.protNoneAll();
        }
        *blocks[i] = i;
    }

    cout << "what is inside of " << *blocks[1] << endl;

    vMem.protNoneAll();

    cout << "what is inside of " << *blocks[18] << endl;
    cout << "what is inside of " << *blocks[19] << endl;
    cout << "what is inside of " << *blocks[5] << endl;
    cout << "what is inside of " << *blocks[8] << endl;
    cout << "what is inside of " << *blocks[1] << endl;

    vMem.deleteInterval();
    cout << "################### end of Code ######################" << endl;
}