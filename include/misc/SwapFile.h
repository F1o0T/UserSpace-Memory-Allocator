#ifndef SWAPFILE_H
#define SWAPFILE_H


#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include "misc/RandomAccessFile.h"



using namespace std;



class SwapFile : public RandomAccessFile
{

public:
    int fd;

    SwapFile()
    {
        fd = open("SwapFile", O_RDWR | O_CREAT | O_TRUNC);
        if (fd == -1)
        {
            cerr << "Error Openining the swap file";
            exit(1);
        }
    }

    ~SwapFile()
    {
        close(fd);
        if (unlink("SwapFile") == -1)
        {
            cerr << "Error unlink didn't delete the swapfile" << endl;
        }
    }

    /**
            This function reads a specified amount of bytes from file to memory.
            Beginning at the offset in the file.
 
            @param addr is the address in memory, where the data is copied to
            @param offset is the start position in the file, where the data is copied from
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
    virtual ssize_t swapFileRead(void *addr, off_t offset, size_t bytes)
    {
        lseek(fd, offset, SEEK_SET);
        read(fd, addr, bytes);
        return bytes;
    }

    /**
            This function writes a specified amount of bytes from memory to file.
            Beginning at the offset in the file.

            @param addr is the address in memory, where the data are copied from
            @param offset is the start position in the file, where the data are copied to
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
    virtual ssize_t swapFileWrite(void *addr, off_t offset, size_t bytes)
    {
        lseek(fd, offset, SEEK_SET);
        write(fd, addr, bytes);
        return 0;
    }

    /**
            This function reserves a specified amount of bytes.
            Beginning at the offset in the file.

            @param offset is the position in file, where reservation starts
            @param bytes is the amount of bytes, which should be reserved
            @return value is the amount of bytes which could be reserved
        */
    virtual ssize_t swapFilereserve(off_t offset, size_t bytes)
    {
        return 0;
    }
};


#endif