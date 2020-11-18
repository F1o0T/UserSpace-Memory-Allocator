#include <iostream>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include <vector>
#include "gui/MemoryGUI.h"

using namespace std;
using namespace GUI;

int main(int argc, char** argv)
{
	FixedMemory<1024> mem;
	FixedHeap<sizeof(int)> heap(mem);
	
	void* ptr = heap.alloc(sizeof(int)*10);
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*5);
	
	heap.free(ptr);
	
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*5);
	heap.alloc(sizeof(int)*10);
	heap.alloc(sizeof(int)*70);
	void* ptr2 = heap.alloc(sizeof(int)*100);
	
	heap.free(ptr2);
	
	heap.alloc(1);

	DrawingWindow window(800,600,"GUI");
	MemoryGUI gui(&heap, &window);
	gui.drawHeapMemory();
	
		
	char input = ' ';
	size_t input2;
	while(input != 'q') {
		cout << "Press q and enter to quit, a to allocate memory, f to free memory" << endl;
		cin >> input;
		if(input == 'a'){
			cout << "Insert number of Bytes to allocate, please:" << endl;
			cin >> input2;
			heap.alloc(input2);
			gui.clearWindow();
			gui.drawHeapMemory();
		}
		if(input == 'f'){
			cout << "Insert address of memory to free, please: " << endl;
			cin >> input2;
			heap.free((void*)input2);
			gui.clearWindow();
			gui.drawHeapMemory();
		}
		cout << "read: '" << input << "' from console" << endl;
	}
	
	return 0;
}
