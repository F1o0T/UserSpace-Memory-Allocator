#include <iostream>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include <vector>

using namespace std;
using namespace GUI;

int main(int argc, char** argv)
{
	int x = 5;
	int y = 10;
	
    DrawingWindow window(800,600,"Memory");
	
	FixedMemory<1024> mem;
	FixedHeap<sizeof(int)> heap(mem);
	
	void* ptr1 = heap.alloc(sizeof(int)*10);
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*5);
	
	heap.free(ptr1);
	
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*50);
	heap.alloc(sizeof(int)*5);
	heap.alloc(sizeof(int)*10);
	heap.alloc(sizeof(int)*70);
	void* ptr2 = heap.alloc(sizeof(int)*100);
	
	heap.free(ptr2);
	
	heap.alloc(1);
	
	window.setBackgroundColor(RGBColor(0,0,0));
	window.setForegroundColor(RGBColor(255,255,255));
	
	for (int i = 0; i < 40; i++) {
		window.drawText(x,y,to_string(i+1));
		x += 20;
	}
	
	x = 5;
	y = 15;
	bool col = 0;
	
	//normale Version mit nur Blöcken
	for (int i = 0; i < heap.getSize(); i += 2) {
		if (heap.getList(i)) {
			if (i != 0) {
				if (!heap.getList(i-1)) {
					col = !col;
				}
			}
			
			if (col) {
				window.setForegroundColor(RGBColor(255,0,0));
			} else {
				window.setForegroundColor(RGBColor(255,165,0));
			}
			
		} else {
			window.setForegroundColor(RGBColor(0,255,0));
		}
		
		if (x > 800) {
			x -= 800;
			y += 15;
		}
		
		window.drawFilledRectangle(x,y,10,10);
		x += 20;
	}
	

	// run until user presses q on console
	char input = ' ';
	while(input != 'q')
	{
		cout << "Press q and enter to quit" << endl;
		cin >> input;
		cout << "read: '" << input << "' from console" << endl;
	}

	return 0;
}

