#include "system/MappedMemory.h"

MappedMemory::MappedMemory() {
    fd = shm_open("mapped_memory_file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) perror("shm_open failed");

    this -> length = pagesize;
    if (ftruncate(fd, length) == -1) perror("ftruncate failed");
}

void MappedMemory::initMem() {
    this -> memblock = mmap(sbrk(0), length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (memblock == MAP_FAILED) perror("mmap failed");
}

MappedMemory::~MappedMemory() {
    close(fd);
    munmap(memblock, length);
    shm_unlink("mapped_memory_file");
}

void* MappedMemory::getStart() {
    return this -> memblock;
}

size_t MappedMemory::getSize() {
    return length;
}

void* MappedMemory::expand(size_t size) {
    size = ((size/pagesize) + 1) * pagesize;
    //msync(memblock, length, MS_SYNC);

    if (ftruncate(fd, length + size) == -1) {
        perror("ftruncate failed");
        return 0;
    }
    
    if (munmap(memblock,length) == -1) {
        perror("munmap failed");
        return 0;
    }

    memblock = mmap(memblock, length + size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (memblock == MAP_FAILED) {
        perror("mmap failed");
        return 0;
    }

    char* begin = (char*) memblock;
    begin += length;

    this -> length += size;

    return begin;
}