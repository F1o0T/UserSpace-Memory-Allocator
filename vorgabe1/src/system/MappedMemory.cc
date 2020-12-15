#include "system/MappedMemory.h"

MappedMemory::MappedMemory(unsigned int startSize) {
}

void MappedMemory::initMem() {
    fd = shm_open("mapped_memory_file", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) perror("shm_open failed");

    this -> length = pagesize;
    if (ftruncate(fd, length) == -1) perror("ftruncate failed");

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

    printf("What2 %ld\n", ((size_t) memblock) % pagesize);
    if (ftruncate(fd, length + size) == -1) {
        perror("ftruncate failed");
        return 0;
    }

    //memblock = mremap(memblock, length, length + size, MREMAP_FIXED);
    
    printf("What3 %p\n", memblock);
    if (munmap(memblock,length) == -1) {
        perror("munmap failed");
        return 0;
    }

    printf("What4 %ld\n", length + size);
    memblock = mmap(memblock, length + size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (memblock == MAP_FAILED) {
        perror("mmap failed");
        return 0;
    }

    char* begin = (char*) memblock;
    begin += length;

    this -> length += size;
    printf("What5 %ld\n", length);

    return begin;
}