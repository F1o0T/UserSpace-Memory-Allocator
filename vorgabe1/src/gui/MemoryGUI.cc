#include "gui/MemoryGUI.h"
#include <list>

	MemoryGUI::MemoryGUI(Heap* heap, DrawingWindow* window) : heap(heap), window(window) {}


    void MemoryGUI::drawHeapMemory(){
        int x = 5;
	    int y = 15;
        bool col = 0;
		
		window -> setBackgroundColor(RGBColor(0,0,0));
		window -> setForegroundColor(RGBColor(255,255,255));
		
		for (int i = 0; i < 40; i++) {
			window -> drawText(x,y,to_string(i+1));
			x += 20;
		}


        for (int i = 0; i < heap -> getSize(); i += 2) {
		    if (heap -> getList(i)) {
			    if (i != 0) {
				    if (!(heap -> getList(i-1))) {
					    col = !col;
				    }
			    }
			
			    if (col) {
				    window -> setForegroundColor(RGBColor(255,0,0));
			    } else {
				    window -> setForegroundColor(RGBColor(255,165,0));
			    }
			
			} else {
				window -> setForegroundColor(RGBColor(0,255,0));
			}
		
			if (x > 800) {
				x -= 800;
				y += 15;
			}
		
			window -> drawFilledRectangle(x,y,10,10);
			x += 20;
		}
    }
