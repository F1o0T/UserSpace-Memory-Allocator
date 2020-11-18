#include <iostream>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include <vector>
#include "gui/MemoryGUI.h"

using namespace std;
using namespace GUI;

#define width 800
#define hight 600
#define blockSize 4
#define memSize 1024

FixedMemory<memSize> mem;
FixedHeap<blockSize> heap(mem);


int main(int argc, char** argv)
{
	
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
	
	heap.alloc(1.45);
	heap.alloc(0);

	DrawingWindow window(width,hight,"GUI");
	MemoryGUI gui(&heap, &window);
	gui.drawHeapMemory();
	
		
	char input = ' ';
	while(input != 'q') {
		cout << "Press q and enter to quit" << endl;
		cin >> input;
		cout << "read: '" << input << "' from console" << endl;
	}
	
	return 0;
}
