#include <iostream>
//#include "gui/DrawingWindow.h"
#include "system/FixedMemory.h"

using namespace std;
//using namespace GUI;

int main(int argc, char** argv)
{
    // gui example code
    //DrawingWindow window(800,600,"gui");

    //window.setBackgroundColor(RGBColor(0,0,0));

    //window.setForegroundColor(RGBColor(0,0,255));
    //window.drawFilledRectangle(100,100,100,100);

    //window.setForegroundColor(RGBColor(0,255,0));
    //window.drawRectangle(250,250,100,150);
    
    //window.drawText(10,200,"test");

	// run until user presses q on console
	//char input = ' ';
	//while(input != 'q')
	//{
	//	cout << "Press q and enter to quit" << endl;
	//	cin >> input;
	//	cout << "read: '" << input << "' from console" << endl;
	//}

	//return 0;

	FixedMemory<1024> mem;
	return 0;
}

