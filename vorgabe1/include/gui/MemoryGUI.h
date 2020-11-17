#ifndef MemoryGUI_h
#define MemoryGUI_h

#include <sys/types.h>

#include "system/Memory.h"
#include "runtime/Heap.h"
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include <list>

using namespace std;
using namespace GUI;

class MemoryGUI {


public:
	MemoryGUI(Heap* heap, DrawingWindow* window);

	void drawHeapMemory();

	
private:
    Heap* heap;
    DrawingWindow* window;
};


#endif
