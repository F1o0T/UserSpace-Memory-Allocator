#include <iostream>
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include "system/BSSMemory.h"
#include "system/MappedMemory.h"
#include "runtime/FirstFitHeap.h"
#include "gui/MemoryGUI.h"
#include <climits>

using namespace std;

#define width 800
#define height 600
#define blockSize 50
#define memSize 1024
#define GUIClass true //true = FirstFitHeap, false = FixedHeap

MappedMemory mem;
//BSSMemory mem(memSize);
//FixedMemory<memSize> mem;

FirstFitHeap heap(mem);
//FixedHeap<blockSize> heap(mem);

DrawingWindow window(width,height,"GUI");
MemoryGUI gui(&heap, &window);

int main(int argc, char** argv)
{
	void* ptr;

	mem.initMem();
	heap.initHeap();

	gui.drawMemory(GUIClass);

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
				cerr << "Input failed!" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');

			} else {
				ptr = heap.alloc(input2);
				cout << "ptr1 is " << ptr <<endl;
				
				gui.clearWindow();
				gui.drawMemory(GUIClass);
			}
		}
		if(input == 'f'){
			cout << "Insert address of memory to free, please: " << endl;
			cin >> input3;

			if (cin.fail()) {
				cerr << "Input failed!" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');

			} else {
				heap.free(input3);
				gui.clearWindow();
				gui.drawMemory(GUIClass);
			}
		}

		cout << endl;
	}
	
	return 0;
}
