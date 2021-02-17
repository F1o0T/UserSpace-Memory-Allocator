#include <iostream>
#include <signal.h>
#include "system/MappedChunk.h"
#include "gui/MemoryGUI.h"
#include "timer/CycleTimer.h"

using namespace std;

#define width 800
#define height 600

#define blockSize 50
#define memSize 10000

#define DEFAULT_CHUNKSIZE 4096
#define MAPPING_SIZE  6 * 4096
#define CHUNKS_NUMBER 6
#define NUMBER_ACTIVE_CHUNKS 3

MappedChunk chunk; 

DrawingWindow window(width, height, "GUI");
MemoryGUI gui(&window, &chunk, MO_CHUNK);

void signalHandeler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    if(info->si_code == SEGV_ACCERR)
    {   
        // cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        chunk.fixPermissions(info->si_addr);
        // cout << "|>>> Permissions were fixed!" << endl; 
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }

} 

int main(int argc, char** argv)
{
	system("clear"); 
	chunk.mappedChunkSet(DEFAULT_CHUNKSIZE, CHUNKS_NUMBER, DEFAULT_CHUNKSIZE, NUMBER_ACTIVE_CHUNKS, 0);
	///////////////////////////////////////////
    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandeler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
	///////////////////////////////////////////
	chunk.printChunkStarts();
	gui.drawMemory();
	CycleTimer t;

	unsigned long address;
	while(1)
	{
			cout << "|>>> Insert an address(or any char to quit): ";
			cin >> address;
			if (cin.fail()) {
				cerr << "wrong input" << endl;
				return 0;
			}
			char decision;
			while (1) {
				cout << "|>>> Insert an r for read and an w for write: ";
				cin >> decision;

				if (decision == 'r') {
    				t.start();
					decision = *((char*)address ); //to update premission
    				cout << "time spent = " << t.stop() << endl;
					cout << "|>>> Read the char " << decision << " from " << address << endl;
					break;

				} else if (decision == 'w') {
					cout << "|>>> Write a char: "; char ch; 
					cin >> ch; 
					t.start();
					*( (char*) address ) = ch;
    				cout << "time spent = " << t.stop() << endl;
					cout << "|>>> " << *((char*)address ) << " successfully written to " << address << endl; 
					break;
				}
			}
			cout << "######################################" <<endl;
			chunk.displayChunks(); 
			cout << "######################################" <<endl;
			gui.drawMemory();
	}		
	return 0;
}