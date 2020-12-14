#include <iostream>
#include <signal.h>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include "system/BSSMemory.h"
#include "runtime/FirstFitHeap.h"

#include "system/MappedChunk.h"

#include <vector>
#include "gui/MemoryGUI.h"
#include <climits>

using namespace std;
using namespace GUI;

#define width 800
#define height 600
#define blockSize 50
#define memSize 10000
//true = FirstFitHeap, false = FixedHeap
#define GUIClass true 


MappedChunk chunk(600, 6, 3); 
void MappedChunk::SignalHandeler(int sigNUmber)
{
    write(STDERR_FILENO, "|<<<| Error: Permission issues!, lets fix it.\n\0", 48); 
    chunk.FixPermissions();
} 

int main(int argc, char** argv)
{
	system("clear"); 
	struct sigaction SigAction;
	SigAction.sa_handler = chunk.SignalHandeler; 
	sigaction(SIGSEGV, &SigAction, NULL);
	chunk.printChunkStarts();

	// gui.drawMemory(GUIClass);
	unsigned long address;
	while(1)
	{

			cout << "|>>>  Insert an address: ";
			cin >> address;
			cout << "|>>>  Write a char: "; char ch; 
			cin >> ch; 
			*( (char*) address ) = ch; 
			cout << "|###  The written char is: " <<  *( (char*) address ) <<endl; 
	}	
	return 0;
}


























































// BSSMemory mem(memSize);
// FixedMemory<memSize> mem;

// FirstFitHeap heap(mem);
// FixedHeap<blockSize> heap(mem);

// DrawingWindow window(width,height,"GUI");
// MemoryGUI gui(&heap, &window);