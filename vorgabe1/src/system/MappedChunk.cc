#include "system/MappedChunk.h"
#include <signal.h>
#include <vector>
#include <algorithm>
MappedChunk::MappedChunk(size_t Size, size_t chunksNumber, size_t maxActChunks)
{

    if((Size % chunksNumber == 0) && (Size != 0) && (maxActChunks <= chunksNumber)){
        memblock = mmap(NULL, Size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,-1,0);
        this->chunksNumber = chunksNumber;
        this->maxActChunks = maxActChunks;
        this->chunkSize = Size / chunksNumber; 
        this->chunksTable = vector<bool>(chunksNumber, false);
        for (int i = 0; i < chunksNumber; i++)
        {
            cout << chunksTable.at(i) << "\t";  
        }
        cout << endl;
    }
    else
    {
        cerr << "Error occured" << endl;
        exit(1);
    }
}

void MappedChunk::FixPermissions()
{
    this->MapSomeoneOut();
    mprotect(this->memblock, this->chunkSize, PROT_READ | PROT_WRITE);
    cout << "|<<<| Permissions were fixed!" << endl; 
}

void MappedChunk::MapSomeoneOut()
{
    std::vector<bool>::iterator it = std::find(this->chunksTable.begin(), this->chunksTable.end(), 0);
    int index = std::distance(this->chunksTable.begin(), it);
    cout << "Index is: " << index << endl;

}

void* MappedChunk::getStart()
{

    return this-> memblock;
}

size_t MappedChunk::getSize()
{

    return chunkSize * chunksNumber;
}

void MappedChunk::printChunkStarts()
{
    for(unsigned int i = 0; i < this -> chunksNumber; i++)
    {
        cout << reinterpret_cast<unsigned long>(memblock) + i * this->chunkSize <<endl;
    }
}

void* MappedChunk::expand(size_t size)
{
    /*
    i dont think that it makes sense to implement expand because you could do 3 things:
        -increase the number of chunks, but as long as you dont increase the number of max. act. chunks
        it wouldnt have any effect other than increasing the "amount of space in the hard drive"
            (this is what the inactive chungs simulate)
        -increase the amount of max. active chunks...yeah...
        -increase the space of each chunk, but because the chunks are consecutive to each other you first have to destroy
        (at least) all but the first one, increase the memory of the first one for some amount and than rebuild the other with
        the new memory size...this also dont seem like it makes sense for this simulation
    */
    return NULL;
}

MappedChunk::~MappedChunk()
{

    munmap(this->memblock, this->chunksNumber * this -> chunkSize);
}


// void MappedChunk::fixPermissions()
// {
//     mprotect(this->memblock, this->chunkSize, PROT_READ | PROT_WRITE);
// }

// void* MappedChunk::activateChunk(){
//     int i = 0;

//     while(chunkList[i] && i < chunkList.size()){
//         i++;
//     }

//     void* address = (void*) (i *chunkSize + ((char*) (memblock)));
//     mprotect(address, chunkSize, PROT_READ);
// }