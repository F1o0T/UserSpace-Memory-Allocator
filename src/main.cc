#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"
#include <string>

#include "gui/MemoryGUI.h"
#include "system/VirtualMem.h"

// size of a chunk
static const unsigned DEFAULT_CHUNKSIZE = 4096;

//VirtualMem to simulate logical and physical memory
VirtualMem vMem;

//file to store the measured times
std::ofstream myfile;

DrawingWindow* window;
MemoryGUI* gui;

bool showGUI = false;

// a simple version of bubble sort
void bubbleSort(unsigned** array, unsigned nrElements)
{
    for (unsigned i = 0; i < nrElements; i++) {
        for (unsigned j = i; j < nrElements; j++) {
            if (*(array[i]) > *(array[j])) {
                int temp = *(array[i]);
                *(array[i]) = *(array[j]);
                *(array[j]) = temp;
            }
            if (showGUI) {
				gui -> drawMemory();	
				sleep(1);			
				//cout << "|>>> Write a char: "; char ch; 
				//cin >> ch; 
		    }
        }
    }
}



void signalHandeler(int sigNUmber, siginfo_t *info, void *ucontext)
{
    if(info->si_code == SEGV_ACCERR)
    {   
        // cout << "|>>> Error: Permission issues!, lets fix it." <<endl;
        vMem.fixPermissions(info->si_addr);
        // cout << "|>>> Permissions were fixed!" << endl;
        /*
        if (showGUI) {
				gui -> drawMemory();	
				sleep(1);			
				//cout << "|>>> Write a char: "; char ch; 
				//cin >> ch; 
		}
        */
    }
    else if(info->si_code == SEGV_MAPERR)
    {
        cout << "|### Error: Access denied, unmapped address!" << endl; 
        exit(1);
    }

} 



/**
 * A simple benchmark for mapped chunk.
 *
 * It creates an array of numbers and sorts them.
 * The numbers are distributed among the whole memory.
 * Each of them resides on its own block and the block size,
 * which means the distance between elements, can be specified.
 *
 * This benchmark accepts a set of options.
 * Use --help to see a list of them.
 */
int main(int argc, char** argv)
{
    // //system("clear"); 
    // /** 
    //  * parse command line arguments
    //  */
    // namespace po = boost::program_options;
    // // define available arguments
    // po::options_description desc("Options");
    // desc.add_options()
    //     ("help,h", "Print help message")
    //     ("total-chunks,t", po::value<uint64_t>()->required(), "Number of chunks (total)")
    //     ("max-chunks,m", po::value<uint64_t>()->required(), "Number of chunks available at the same time)")
    //     ("pinned-chunks,p", po::value<uint64_t>()->required(), "Number of pinned chunks")
    //     ("blocksize,b", po::value<uint64_t>()->required(), "Size of a block (determines access pattern)")
    //     (",r", po::value<uint64_t>()->default_value(10), "Repeat benchmark <arg> times")
    //     ("wball,w", "Write everything back (otherwise only dirty chunks are written back")
    //     (",o", po::value<string>()->default_value("values.csv"),"Name of the output file")
    //     ("gui,g", "Enable GUI"); 

    // // read arguments
    // po::variables_map vm;
    // po::store(po::parse_command_line(argc, argv, desc), vm);
    // // did someone say 'help'?
    // if (vm.count("help")) {
    //     std::cerr << desc << std::endl;
    //     return 0;
    // }
    // read arguments into variables
    //po::notify(vm);
    // uint64_t totalChunks = vm["total-chunks"].as<uint64_t>();
    // uint64_t maxChunksAvailable = vm["max-chunks"].as<uint64_t>();
    //uint64_t pinnedChunks = vm["pinned-chunks"].as<uint64_t>();
    // uint64_t blockSize = vm["blocksize"].as<uint64_t>();
    // uint64_t runs = vm["-r"].as<uint64_t>();
    // string outputFileName = vm["-o"].as<string>();
    // ///////////////////////////////////////////////////////////////////////////////////
    // if (vm.count("gui")) {
    //     showGUI = true;
    // }
    bool writeBackAll = false;
    uint64_t pinnedChunks = 0;
    // if (vm.count("wball")) {
    //     writeBackAll = true;
    // }
    /**
     * parsing command line arguments done.
     * create memory with the above 
     * read configuration.
     * 
     */
    vMem.initializeVirtualMem(pinnedChunks, writeBackAll);
    
    // if (showGUI) {
    //     //window = new DrawingWindow(width, height, "GUI");
    //     //gui = new MemoryGUI(&vMem, window, MO_CHUNK);
    // }
    // ///////////////////////////////////////////
    // struct sigaction SigAction;
    // SigAction.sa_sigaction = signalHandeler;
    // SigAction.sa_flags = SA_SIGINFO;
    // sigaction(SIGSEGV, &SigAction, NULL);
    // ///////////////////////////////////////////
    // std::cout << "# totalChunks,maxChunksAvailable,blockSize,writeBackAll,times" << std::endl;
    // std::cout << totalChunks << "," << maxChunksAvailable << "," << blockSize << "," << writeBackAll << "," << runs << std::endl;

    // //creating the file, where the values shall be stored
    // //////////////////////////////////////////
    // myfile.open(outputFileName, std::ofstream::trunc);
    // //////////////////////////////////////////

    // // each element has a size of blockSize
    // // how many can we store in the memory?
    // unsigned nrElements = (totalChunks*DEFAULT_CHUNKSIZE)/blockSize;
    // // this array stores pointers to the blocks
    // unsigned* blocks[nrElements];

    // unsigned randNumbers[nrElements];

    // // initialize the field 
    // for (unsigned i = 0; i < nrElements; i++) {
    //     randNumbers[i] = i; // already sorted
    //     //randNumbers[i] = std::rand() % 100; // pseudo-random values
    //     //randNumbers[i] = nrElements-i; // reverse sorted
    // }

    // long decreasableMaxChunkNumber = (long) maxChunksAvailable;

    // CycleTimer t;

    // //in each iteration the maxChunksAvailable is decreased by 10, so the sorting should take more
    // //time with every iteration
    // while(decreasableMaxChunkNumber > (long) pinnedChunks)
    // {
    //     //storing time value in the csvfile
    //     myfile << decreasableMaxChunkNumber;

    //     for (uint64_t run = 0; run < runs; run++)
    //     {   
	// 		vMem.resetQueues();
    //         //initialize the VirtualMem with the current maxChunkAvailable
    //         vMem.virtualMemSet(pinnedChunks, writeBackAll);
    //         // store the start address of each block in the array
    //         unsigned* memStart = static_cast<unsigned*>(vMem.getStart());
    //         for (unsigned i = 0; i < nrElements; i++) {
    //             blocks[i] = memStart + (i * blockSize/sizeof(unsigned));
    //         }

    //         // initialize the field
    //         for (unsigned i = 0; i < nrElements; i++) {
    //             *blocks[i] = randNumbers[i]; // pseudo-random values
    //         }

    //         // measure the time
    //         t.start();
    //         bubbleSort(blocks, nrElements);
    //         uint64_t time = t.stop();

    //         myfile << "," << time;
            
    //         asm volatile("" :: "g"(&blocks), "g"(&nrElements) : "memory");
    //     	if (showGUI) break;
    //     }
    //     myfile << "\n";
    //     //decrease
    //     decreasableMaxChunkNumber -= 10;
    // 	//mem.displayChunks();
    // }
    // myfile << "\n";
    // myfile << "writeBackAll = " << writeBackAll << "\n";
    // myfile.close();
    // std::cout << std::endl;

    // if (showGUI) {
    //     cout << "|>>> Write a char to close: "; char ch; 
	// 	cin >> ch;
    //     delete(window);
    //     delete(gui);
    // }
    return 0;
}