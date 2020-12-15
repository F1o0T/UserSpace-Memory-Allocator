#include <iostream>
#include "runtime/FixedHeap.h"
#include "system/FixedMemory.h"
#include "system/BSSMemory.h"
#include "system/MappedMemory.h"
#include "runtime/FirstFitHeap.h"
#include "gui/MemoryGUI.h"
#include <climits>

using namespace std;

#define width 800
#define height 600
#define blockSize 50
#define memSize 1024
#define GUIClass true //true = FirstFitHeap, false = FixedHeap


//BSSMemory mem(memSize);
//FixedMemory<memSize> mem;
MappedMemory mem(memSize);

FirstFitHeap heap(mem);
//FixedHeap<blockSize> heap(mem);


DrawingWindow window(width,height,"GUI");
MemoryGUI gui(&heap, &window);

void lol() {	
	int fd = shm_open("shm-file", O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) perror("shm_open failed");
	void* addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == (void*)-1) perror("mmap failed");
	int x = *((int*)addr);
	printf("0x%.2x\n", x);
	*((int*)addr) = 0xbeef;
}


int main(int argc, char** argv)
{
	mem.initMem();
	heap.initHeap();
	int fd;
	fd = shm_open("shm-file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) perror("shm_open failed");
	if (ftruncate(fd, 4096) == -1) perror("ftruncate failed");
	/*void* addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == (void*)-1) perror("mmap failed");
	*((int*)addr) = 0xcafe;
	while (*((int*)addr) == 0xcafe) {
		lol();
	}
	printf("0x%.2x\n", *((int*)addr));
	if (shm_unlink("shm-file") == -1) perror("shm_unlink failed");*/

	void* addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (addr == (void*)-1) perror("mmap failed");
	int number = *(int*)addr;
	char* buffer = (char*)(((int*)addr) + 1);
	printf("data: %d, %s\n", number, buffer);



	void* ptr;

	gui.drawMemory(GUIClass);

	char input = ' ';
	int input2;
	void* input3;
	while(input != 'q') {
		cout << "Press q and enter to quit, a to allocate memory, f to free memory" << endl;
		cin >> input;
		cin.ignore(INT_MAX, '\n');

		if(input == 'a'){
			cout << "Insert number of Bytes to allocate, please:" << endl;
			cin >> input2;
			
			if (cin.fail() || input2 < 0) {
				cerr << "Input failed!" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');

			} else {
				ptr = heap.alloc(input2);
				cout << "ptr1 is " << ptr <<endl;
				
				gui.clearWindow();
				gui.drawMemory(GUIClass);
			}
		}
		if(input == 'f'){
			cout << "Insert address of memory to free, please: " << endl;
			cin >> input3;

			if (cin.fail()) {
				cerr << "Input failed!" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');

			} else {
				heap.free(input3);
				gui.clearWindow();
				gui.drawMemory(GUIClass);
			}
		}

		cout << endl;
	}
	
	return 0;
}
