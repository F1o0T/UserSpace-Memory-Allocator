#include "system/MappedChunk.h"



MappedChunk::MappedChunk(size_t startSize){

    if(startSize% 2 == 0 && startSize != 0){
        memblock = mmap(NULL, startSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,-1,0);            //map the whole memoryblock to be clear it can be used
        munmap(memblock, startSize);
        mmap(memblock, startSize/2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS| MAP_FIXED,-1,0);            //map first chunk
        char* chunkStart = ((char* )memblock) + startSize/2;
        mmap(chunkStart, startSize/2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS| MAP_FIXED,-1,0);            //map second chunk
        chunks = 2;
        maxactChunks = 1;
        chunkSize = startSize/2;
        chunkList = vector<bool>(chunks, false);
    }else{
       cerr << "startSize has to be dividable by number of chunks (default = 2)" << endl;
       memblock = NULL;
       chunks = 0;
       maxactChunks =0;
    }
}

MappedChunk::MappedChunk(size_t startSize, size_t chunks, size_t maxactchunks){

    if((startSize % chunks == 0) && (startSize!= 0) && (maxactchunks <= chunks)){
        memblock = mmap(NULL, startSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,-1,0);            //map the whole memoryblock to be clear it can be used
        munmap(memblock, startSize);

        this->chunkSize = startSize/chunks;
        for(int i = 0; i < (chunks); i++){
            char* chunkStart = ((char* )memblock) + i * startSize/chunks;
            mmap(chunkStart, startSize/chunks,PROT_NONE,MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,-1,0);   //map the chunks
        }

        this->chunks = chunks;
        this->maxactChunks = maxactchunks;
        chunkList = vector<bool>(chunks, false);

    }else{
        cerr << "startSize has to be dividable by number of chunks" << endl;
        memblock = NULL;
        chunks = 0;
        maxactChunks =0;
        chunkList = vector<bool>(chunks);
    }

}

	
void* MappedChunk::getStart(){
    return this-> memblock;
}

size_t MappedChunk::getSize(){
    return chunkSize * chunks;
}

void* MappedChunk::activateChunk(){
    int i = 0;

    while(chunkList[i] && i < chunkList.size()){
        i++;
    }

    void* address = (void*) (i *chunkSize + ((char*) (memblock)));
    mprotect(address, chunkSize, PROT_READ);
}




void* MappedChunk::expand(size_t size){
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


}