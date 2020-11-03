#ifndef Heap_h
#define Heap_h

#include <sys/types.h>

#include "system/Memory.h"
/**
 * A heap manages the memory it gets from the system.
 * The application can allocate and free memory pieces.
 */
class Heap {
protected:
	/**
	 * Initializes reference to system memory.
	 */
	Heap(Memory& memory) : memory(memory)
	{
	}

public:
	/**
	 * Allocate a piece of memory.
	 *
	 * @param size size of the memory to allocate
	 * @return pointer to allocated memory or null on error
	 */
	virtual void* alloc(size_t size)=0;

	/**
	 * Free previously allocated memory.
	 *
     * @param p pointer to the memory to free
	 */
	virtual void free(void* address)=0;

protected:
	/**
	 * Reference to the system memory source.
	 */
	Memory& memory;

};

#endif

