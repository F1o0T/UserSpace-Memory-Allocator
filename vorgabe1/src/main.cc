#include <iostream>
#include "gui/DrawingWindow.h"
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include <vector>

using namespace std;
using namespace GUI;

int main(int argc, char** argv)
{
    // gui example code
    DrawingWindow window(800,600,"gui");

    //window.setBackgroundColor(RGBColor(0,0,0));

    //window.setForegroundColor(RGBColor(0,0,255));
    //window.drawFilledRectangle(100,100,100,100);

    //window.setForegroundColor(RGBColor(0,255,0));
    //window.drawRectangle(250,250,100,150);
    
    window.drawText(10,200,"test");
	
	FixedMemory<1024> mem;
	FixedHeap<sizeof(int)> heap(mem);
	
	cout << "Anzahl der Bytes des gesamten Speichers: " << mem.getSize() << endl;
	cout << "Anzahl der Bloecke: " << heap.getBlockCount() << endl;

	heap.alloc(8);
	cout << "Anzahl der freien Bloecke nach alloc: " << heap.freeBlocks() << endl;
	for(int i = 0; i < 9;i++){
		cout << heap.getList(i) << endl;
	}
	heap.free(mem.getStart());
	cout << "Anzahl der freien Bloecke nach free: " << heap.freeBlocks() << endl;

	//for(int i = 0; i < heap.getBlockCount(); i++){
	//	cout << heap.getList(i) << endl;
	//}

	heap.alloc(sizeof(int)*10);
	//cout << mem.getSize() << endl;
	//cout << heap.getList() << endl;
	/*
	for (int i = 0; i < (int) (1024/sizeof(int)); i++) {
		cout << heap.getList(i) << endl;
	}*/

	// run until user presses q on console
	//char input = ' ';
	//while(input != 'q')
	//{
	//	cout << "Press q and enter to quit" << endl;
	//	cin >> input;
	//	cout << "read: '" << input << "' from console" << endl;
	//}

	//return 0;
	return 0;
}
