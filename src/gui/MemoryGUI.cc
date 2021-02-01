#include "gui/MemoryGUI.h"

MemoryGUI::MemoryGUI() : heap(0), page(0), mode(MO_PAGE) {}
MemoryGUI::MemoryGUI(Heap* heap, DrawingWindow* window, int mode) : heap(heap), page(0), window(window), mode(mode) {}
MemoryGUI::MemoryGUI(VirtualMem* page, DrawingWindow* window, int mode) : heap(0), page(page), window(window), mode(mode) {}

void MemoryGUI::drawMemory(){
	if(mode == MO_BSSM){
		drawBSSMemory();

	} else if (mode == MO_FIXM) {
		drawFixedHeapMemory();

	} else if (mode == MO_CHUNK) {
		drawMappedChunk();

	} else if (mode == MO_PAGE) {
		drawVirtualMem();
	} 
}

void MemoryGUI::drawBSSMemory(){
	int x = 5;
	int y = 15;
	bool col = true;
	std::list<int> ptr;
	
	window->setBackgroundColor(RGBColor(0,0,0));
	window->setForegroundColor(RGBColor(255,255,255));

	window->drawText(x,y,"Memorysize : ");
	window->drawText(x+80,y,to_string(heap -> getSize()));
	y += 15;

	heap -> fillList(&ptr);

	for (int i : ptr) {
	
		if (x > width) {
			x -= width;
			y += 15;
		}

		if (i == -1) {
			window->setForegroundColor(RGBColor(0,0,255));
			window->drawFilledRectangle(x,y,10,10);
			window->setForegroundColor(RGBColor(255,255,0));
			window->drawText(x+2,y+10,"M");
			col = true;
			x += 20;
		} else if (i == -2) {
			window->setForegroundColor(RGBColor(0,0,255));
			window->drawFilledRectangle(x,y,10,10);
			window->setForegroundColor(RGBColor(255,255,0));
			window->drawText(x+2,y+10,"M");
			col = false;
			x += 20;
		} else {
			if (col) {
				window->setForegroundColor(RGBColor(0,255,0));
				window->drawFilledRectangle(x,y,50,10);
				window->setForegroundColor(RGBColor(255,0,0));
				window->drawText(x+2,y+10,"F :" + to_string(i));
				x += 60;
			} else {
				window->setForegroundColor(RGBColor(255,0,0));
				window->drawFilledRectangle(x,y,50,10);
				window->setForegroundColor(RGBColor(0,255,0));
				window->drawText(x+2,y+10,"B :" + to_string(i));
				x += 60;
			}
		}
	}
}    

void MemoryGUI::drawFixedHeapMemory(){
	int x = 5;
	int y = 15;
	
	window->setBackgroundColor(RGBColor(0,0,0));
	window->setForegroundColor(RGBColor(255,255,255));
	
	for (int i = 0; i < 40; i++) {
		window->drawText(x,y,to_string(i+1));
		x += 20;
	}

	std::list<int> ptr;
	heap -> fillList(&ptr);

	for (int i : ptr) {
		if (i == -1) {
			window->setForegroundColor(RGBColor(255,0,0));
		} else if (i == -2) {
			window->setForegroundColor(RGBColor(255,165,0));
		} else {
			window->setForegroundColor(RGBColor(0,255,0));
		}
	
		if (x > width) {
			x -= width;
			y += 15;
		}
	
		window->drawFilledRectangle(x,y,10,10);
		x += 20;
	}
}

void MemoryGUI::drawMappedChunk() {
	int x = 5;
	int y = 15;
	
	std::list<int> ptr;
	page -> fillList(&ptr);

	if (first == true) {
		window->drawText(x, y, "NONE");
		x += 120;
		window->drawText(x, y, "READ");
		x += 120;
		window->drawText(x, y, "WRITE");

		y = 6;
		x = 45;
		window->setForegroundColor(RGBColor(255,0,0));
		window->drawFilledRectangle(x,y,40,10);
		x += 120;
		window->setForegroundColor(RGBColor(255,165,0));
		window->drawFilledRectangle(x,y,40,10);
		x += 120;
		window->setForegroundColor(RGBColor(0,255,0));
		window->drawFilledRectangle(x,y,40,10);

		y = 35;
		x = 5;

		window->setBackgroundColor(RGBColor(0,0,0));
		window->setForegroundColor(RGBColor(255,255,255));

		for (size_t i = 0; i < ptr.size(); i++) {
			window->drawText(x, y, "page " + to_string(i));
			x += 60;	

			if (x > (width - 20)) {
				x -= (width - 20);
				y += 40;
			}
		}

		this -> first = false;
	}

	y = 40;
	x = 5;

	for (int i : ptr) {
		if (i == 0) {
			window->setForegroundColor(RGBColor(255,0,0));
		} else if (i == 1) {
			window->setForegroundColor(RGBColor(255,165,0));
		} else if (i == 2) {
			window->setForegroundColor(RGBColor(0,255,0));
		}
	
		if (x > (width - 20)) {
			x -= (width - 20);
			y += 40;
		}
	
		window->drawFilledRectangle(x,y,40,10);
		x += 60;
	}
}

void MemoryGUI::drawVirtualMem() {
	int x = 2;
	int y = 5;
	unsigned j = 0;
	
	std::list<int> ptr;
	page -> fillList(&ptr);

	for (int i : ptr) {
		if (i != 0) {
			if (i == 1) {
				window->setForegroundColor(RGBColor(255,165,0));
			} else if (i == 2) {
				window->setForegroundColor(RGBColor(0,255,0));
			}

			if (x > (width - 20)) {
				x -= (width - 20);
				y += 40;
			}

			window->drawFilledRectangle(x,y,30,10);
			window->setForegroundColor(RGBColor(0,0,0));
			window->drawText(x+1, y+10, "p " + to_string(j));
			x += 40;
		}
		j++;
	}
}

void MemoryGUI::clearWindow(){
	window->clear();
}