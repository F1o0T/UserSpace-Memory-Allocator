#include "gui/MemoryGUI.h"
#include <list>



	MemoryGUI::MemoryGUI(Heap* heap) : heap(heap), window(new DrawingWindow(800,600,"Memory"))
	{
        initializeWindow();
        drawHeapMemory();
        
	}

   MemoryGUI::MemoryGUI(Heap* heap, DrawingWindow* window) : heap(heap), window(window)
	{
        //drawHeapMemory();
	}

    void MemoryGUI::initializeWindow(){
        (*window).setForegroundColor(RGBColor(255,0,0));
	    //(*window).drawRectangle(100,100,75,150);
	    (*window).setForegroundColor(LIME);
	    //(*window).drawLine(0,0,100,100);
    }


    void MemoryGUI::drawHeapMemory(){
        int x = 5;
	    int y = 15;
        bool col = 0;

        

        for (int i = 0; i < (*heap).getSize(); i += 2) {
		    if ((*heap).getList(i)) {
			    if (i != 0) {
				    if (!(*heap).getList(i-1)) {
					    col = !col;
				    }
			    }
			
			    if (col) {
				    (*window).setForegroundColor(RGBColor(255,0,0));
			    } else {
				    (*window).setForegroundColor(RGBColor(255,165,0));
			    }
			
		} else {
			(*window).setForegroundColor(RGBColor(0,255,0));
		}
		
		if (x > 800) {
			x -= 800;
			y += 15;
		}
		
		(*window).drawFilledRectangle(x,y,10,10);
		x += 20;
	}

    
    }