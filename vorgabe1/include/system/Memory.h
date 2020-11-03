#ifndef Memory_h
#define Memory_h

#include <sys/types.h>
/**
 * This is an interface for rough granular memory allocations from the
 * system.
 */
class Memory {

    public:
        /**
         * Get start address of the reserved memory region.
         * @return start address
         */
        void* getStart(){
            return this->start;
        }

        /**
         * Get size of the memory region.
         * @return size in bytes
         */
        size_t getSize(){
            return this->size;
        }

        /**
         * Expansion request for the memory.
         *
         * @param size value to expand
         * @return start address of expansion area, null if expansion failed
         */
        virtual void* expand(size_t size) = 0;
    
    protected:
        void* start;
        size_t size;
};

#endif

