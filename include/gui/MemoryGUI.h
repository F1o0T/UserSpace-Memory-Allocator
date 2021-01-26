#ifndef MemoryGUI_h
#define MemoryGUI_h

#include <sys/types.h>
#include "system/Memory.h"
#include "runtime/Heap.h"
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "runtime/FirstFitHeap.h"
#include "system/MappedChunk.h"
#include <list>

using namespace std;
using namespace GUI;

#define MO_BSSM 1
#define MO_FIXM 2
#define MO_CHUNK 3

#define width 800
#define height 600

class MemoryGUI {
public:
    MemoryGUI();
	MemoryGUI(Heap* heap, DrawingWindow* window, int mode);
    MemoryGUI(MappedChunk* chunk, DrawingWindow* window, int mode);

    void drawMemory();
	
private:
	void drawBSSMemory();

    void drawFixedHeapMemory();

    void drawMappedChunk();

    void clearWindow();

    Heap* heap;
    MappedChunk* chunk;
    DrawingWindow* window;
    int mode;
    bool first = true;
};


#endif
