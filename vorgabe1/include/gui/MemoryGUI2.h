#ifndef MemoryGUI2_h
#define MemoryGUI2_h

#include <sys/types.h>
#include "system/Memory.h"
#include "runtime/Heap.h"
#include "gui/DrawingWindow.h"
#include "runtime/FirstFitHeap.h"
#include <list>

using namespace std;
using namespace GUI;

class MemoryGUI2 {


public:
	MemoryGUI2(Heap* heap, DrawingWindow* window);

	void drawHeapMemory();

    void clearWindow();
	
private:
    FirstFitHeap* heap;
    DrawingWindow* window;
};


#endif
