#ifndef Heap_h
#define Heap_h

#include <sys/types.h>
#include <list>
#include "system/Memory.h"
#include <tuple>
/**
 * A heap manages the memory it gets from the system.
 * The application can allocate and free memory pieces.
 */
class Heap {
protected:
	/**
	 * Initializes reference to system memory.
	 */
	/*Heap(Memory& memory) : memory(memory)
	{
	}*/

public:
	/**
	 * Allocate a piece of memory.
	 *
	 * @param size size of the memory to allocate
	 * @return pointer to allocated memory or null on error
	 */
	virtual void* malloc(size_t size)=0;

	virtual void* realloc(void* ptr, size_t size)=0;

	virtual void* calloc(size_t nmemb, size_t size)=0;

	/**
	 * Free previously allocated memory.
	 *
     * @param p pointer to the memory to free
	 */
	virtual void free(void* address)=0;


	virtual int getSize()=0;

	virtual void fillList(std::list<int>* list) = 0;


	//virtual list<tuple<size_t, bool>> getMemoryList()=0;

protected:
	/**
	 * Reference to the system memory source.
	 */
	//static Memory& memory;

};

#endif

