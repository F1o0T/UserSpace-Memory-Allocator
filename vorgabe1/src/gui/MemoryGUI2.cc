#include "gui/MemoryGUI2.h"
#include <list>

	MemoryGUI2::MemoryGUI2(Heap* heap, DrawingWindow* window) : heap((FirstFitHeap*) heap), window(window) {}


    void MemoryGUI2::drawHeapMemory(){
        int x = 5;
	    int y = 15;
		
		window -> setBackgroundColor(RGBColor(0,0,0));
		window -> setForegroundColor(RGBColor(255,255,255));

		window -> drawText(x,y,"Memorysize : ");
		window -> drawText(x+80,y,to_string(heap -> getSize()));
		y += 15;

		
		for (int i = 0; i < 40; i++) {
			window -> drawText(x,y,to_string(i+1));
			x += 20;
		}
/*
		char* ptr = (char*) calloc(heap -> getSize(), sizeof(char));
		heap -> fillArray(ptr);

        for (int i = 0; i < heap -> getSize(); i += 2) {
		
			if (x > 800) {
				x -= 800;
				y += 15;
			}

			switch (ptr[i]) {
				case 'M':
					window -> setForegroundColor(RGBColor(0,0,255));
					window -> drawFilledRectangle(x,y,10,10);
					break;
				case 'F':
					window -> setForegroundColor(RGBColor(0,255,0));
					window -> drawFilledRectangle(x,y,10,10);
					break;
				case 'B':
					window -> setForegroundColor(RGBColor(255,0,0));
					window -> drawFilledRectangle(x,y,10,10);
					break;
				default:
					x -= 20;
			}
		
			x += 20;
		}*/
    }

	void MemoryGUI2::clearWindow(){
		window -> clear();
	}
