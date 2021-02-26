#ifndef MemoryGUI_h
#define MemoryGUI_h

#include <sys/types.h>
#include "system/Memory.h"
#include "gui/DrawingWindow.h"
#include "runtime/FirstFitHeap.h"
#include "system/VirtualMem.h"
#include <list>

using namespace std;
using namespace GUI;

#define MO_BSSM 1
#define MO_FIXM 2
#define MO_CHUNK 3
#define MO_PAGE 4

#define width 1200
#define height 800

class MemoryGUI {
public:
    MemoryGUI();
	//MemoryGUI(Heap* heap, DrawingWindow* window, int mode);
    MemoryGUI(VirtualMem* page, DrawingWindow* window, int mode, unsigned numberOfElements);

    void drawMemory();
	
private:
	void drawBSSMemory();

    void drawFixedHeapMemory();

    void drawMappedChunk();
    
    void drawVirtualMem();

    void clearWindow();

    //Heap* heap;
    VirtualMem* page;
    DrawingWindow* window;
    int mode;
    bool first = true;
    unsigned numberOfElements = 0;
};


#endif
