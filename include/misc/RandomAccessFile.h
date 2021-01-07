#ifndef RandomAccessFile_h
#define RandomAccessFile_h

#include <sys/types.h>

/**
	This class provides an interface, for storing data in a random access way.
	Additionally reservation of space, needed in the future, is supported.
*/
class RandomAccessFile
{

    public:

        /**
            This function reads a specified amount of bytes from file to memory.
            Beginning at the offset in the file.

            @param addr is the address in memory, where the data is copied to
            @param offset is the start position in the file, where the data is copied from
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
        virtual ssize_t read(void* addr, off_t offset, size_t bytes) = 0;

        /**
            This function writes a specified amount of bytes from memory to file.
            Beginning at the offset in the file.

            @param addr is the address in memory, where the data are copied from
            @param offset is the start position in the file, where the data are copied to
            @param bytes is the amount of bytes, which should be copied
            @return value is the amount of bytes, which was copied
        */
        virtual ssize_t write(void* addr, off_t offset, size_t bytes) = 0;

        /**
            This function reserves a specified amount of bytes.
            Beginning at the offset in the file.

            @param offset is the position in file, where reservation starts
            @param bytes is the amount of bytes, which should be reserved
            @return value is the amount of bytes which could be reserved
        */
        virtual ssize_t reserve(off_t offset, size_t bytes) = 0;
};

#endif