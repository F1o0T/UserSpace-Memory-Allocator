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
	gui.clearWindow();
	
		
	char input = ' ';
	while(input != 'q') {
		cout << "Press q and enter to quit" << endl;
		cin >> input;
		cout << "read: '" << input << "' from console" << endl;
	}
	
	return 0;
}
