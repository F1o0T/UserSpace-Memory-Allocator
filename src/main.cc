#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <boost/program_options.hpp>
#include "timer/CycleTimer.h"

#include "gui/MemoryGUI.h"
#include "system/MappedChunk.h"

// size of a chunk
static const unsigned DEFAULT_CHUNKSIZE = 4096;

//MappedChunk to simulate logical and physical memory
MappedChunk mem;

//file to store the measured times
std::ofstream myfile;

#define width 800
#define height 600
DrawingWindow window(width, height, "GUI");
MemoryGUI gui(&window, &mem, MO_CHUNK);

bool showGui = false;

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

			if (showGui) {
				gui.drawMemory();	
				sleep(0.2);			
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
        mem.fixPermissions(info->si_addr);
        // cout << "|>>> Permissions were fixed!" << endl; 
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
    //system("clear"); 
    /** 
     * parse command line arguments
     */
    namespace po = boost::program_options;
    // define available arguments
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help message")
        ("total-chunks,t", po::value<uint64_t>()->required(), "Number of chunks (total)")
        ("max-chunks,m", po::value<uint64_t>()->required(), "Number of chunks available at the same time)")
        ("blocksize,b", po::value<uint64_t>()->required(), "Size of a block (determines access pattern)")
        (",r", po::value<uint64_t>()->default_value(10), "Repeat benchmark <arg> times")
        ("wball,w", "Write everything back (otherwise only dirty chunks are written back")
        ("gui,g", "Enable GUI"); 

    // read arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    // did someone say 'help'?
    if (vm.count("help")) {
        std::cerr << desc << std::endl;
        return 0;
    }
    // read arguments into variables
    po::notify(vm);
    uint64_t totalChunks = vm["total-chunks"].as<uint64_t>();
    uint64_t maxChunksAvailable = vm["max-chunks"].as<uint64_t>();
    uint64_t blockSize = vm["blocksize"].as<uint64_t>();
    uint64_t runs = vm["-r"].as<uint64_t>();
    ///////////////////////////////////////////////////////////////////////////////////
    bool showGUI = false;
    if (vm.count("gui")) {
        showGUI = true;
    }
    bool writeBackAll = false;
    if (vm.count("wball")) {
        writeBackAll = true;
    }
    /**
     * parsing command line arguments done.
     * create memory with the above 
     * read configuration.
     */
    mem.mappedChunkSet(DEFAULT_CHUNKSIZE, totalChunks, DEFAULT_CHUNKSIZE, maxChunksAvailable, writeBackAll);

    ///////////////////////////////////////////
    struct sigaction SigAction;
    SigAction.sa_sigaction = signalHandeler;
    SigAction.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &SigAction, NULL);
    ///////////////////////////////////////////

    if (showGUI) {
        showGui = showGUI;
    }
    std::cout << "# totalChunks,maxChunksAvailable,blockSize,writeBackAll,times" << std::endl;
    std::cout << totalChunks << "," << maxChunksAvailable << "," << blockSize << "," << writeBackAll << std::endl;

    //creating the file, where the values shall be stored
    //////////////////////////////////////////
    myfile.open("values.csv", std::ofstream::trunc);
    //////////////////////////////////////////

    // each element has a size of blockSize
    // how many can we store in the memory?
    unsigned nrElements = (totalChunks*DEFAULT_CHUNKSIZE)/blockSize;
    // this array stores pointers to the blocks
    unsigned* blocks[nrElements];

    unsigned randNumbers[nrElements];

    // initialize the field 
    for (unsigned i = 0; i < nrElements; i++) {
        //*blocks[i] = i; // already sorted
        //randNumbers[i] = std::rand() % 100; // pseudo-random values
        randNumbers[i] = nrElements-i; // reverse sorted
    }

    long decreasableMaxChunkNumber = (long) maxChunksAvailable;
    CycleTimer t;

    //in each iteration the maxChunksAvailable is decreased by 10, so the sorting should take more
    //time with every iteration
    while(decreasableMaxChunkNumber > 0)
    {
        //storing time value in the csvfile
        myfile << decreasableMaxChunkNumber;

        for (uint64_t run = 0; run < runs; run++)
        {   
			mem.reset();
            //initialize the MappedChunk with the current maxChunkAvailable
            mem.mappedChunkSet(DEFAULT_CHUNKSIZE, totalChunks, DEFAULT_CHUNKSIZE, decreasableMaxChunkNumber, writeBackAll);

            // store the start address of each block in the array
            unsigned* memStart = static_cast<unsigned*>(mem.getStart());
            for (unsigned i = 0; i < nrElements; i++) {
                blocks[i] = memStart + (i * blockSize/sizeof(unsigned));
            }

            // initialize the field
            for (unsigned i = 0; i < nrElements; i++) {
                *blocks[i] = randNumbers[i]; // pseudo-random values
            }

			/*for (unsigned* n : blocks) {
                cout << *n << ", ";
            }
			cout << endl;*/

            // measure the time
            t.start();
            //auto start = std::chrono::high_resolution_clock::now();
            //asm volatile("" ::: "memory");
            bubbleSort(blocks, nrElements);
            //asm volatile("" ::: "memory");
            //auto end = std::chrono::high_resolution_clock::now();
            uint64_t time = t.stop();
        

            // options: milli, micro or nano
            //long time = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

            myfile << "," << time;

            /*for (unsigned* n : blocks) {
                cout << *n << ", ";
            }
			cout << endl;*/
            
            asm volatile("" :: "g"(&blocks), "g"(&nrElements) : "memory");
        	if (showGUI) break;
        }

        myfile << "\n";

        //decrease
        decreasableMaxChunkNumber -= 10;
        cout << "ok 1 = " << endl;
    	mem.displayChunks();
    }

    std::cout << std::endl;

    if (showGUI) {
        cout << "|>>> Write a char: "; char ch; 
		cin >> ch; 
    }
    return 0;
}