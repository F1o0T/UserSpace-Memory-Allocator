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

	MemoryGUI gui(&heap);
	gui.drawHeapMemory();
	

	return 0;
}
