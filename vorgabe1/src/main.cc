#include <iostream>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include "system/BSSMemory.h"
#include "runtime/FirstFitHeap.h"
#include <vector>
#include "gui/MemoryGUI.h"
#include <climits>

using namespace std;
using namespace GUI;

#define width 800
#define hight 600
#define blockSize 4
#define memSize 1024

BSSMemory mem;
FirstFitHeap heap(mem);

//FixedMemory<memSize> mem;
//FixedHeap<blockSize> heap(mem);

DrawingWindow window(width,hight,"GUI");
MemoryGUI gui(&heap, &window);


int main(int argc, char** argv)
{
	void* ptr;
	mem.initMem();
	heap.initHeap(memSize);

	gui.drawHeapMemory();

	char input = ' ';
	int input2;
	void* input3;
	while(input != 'q') {
		cout << "Press q and enter to quit, a to allocate memory, f to free memory" << endl;
		cin >> input;
		cin.ignore(INT_MAX, '\n');

		if(input == 'a'){
			cout << "Insert number of Bytes to allocate, please:" << endl;
			cin >> input2;
			
			if (cin.fail() || input2 < 0) {
				cerr << "Eingabe ist fehlgeschlagen!" << endl;
				cin.clear();

			} else {
				ptr = heap.alloc(input2);
				cout << "ptr1 is " << ptr <<endl;
				
				gui.clearWindow();
				gui.drawHeapMemory();
			}
		}
		if(input == 'f'){
			cout << "Insert address of memory to free, please: " << endl;
			cin >> input3;

			if (cin.fail()) {
				cerr << "Eingabe ist fehlgeschlagen!" << endl;
				cin.clear();

			} else {
				heap.free(input3);
				gui.clearWindow();
				gui.drawHeapMemory();
			}
		}
		
		cout << endl;
	}
	
	return 0;
}
