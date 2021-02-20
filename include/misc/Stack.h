#ifndef STACK_H
#define STACK_H

#include <iostream>

using namespace std;


class StackPageNode
{
public:
    void *pageAddress;
    StackPageNode *prev, *next;

    void* operator new(size_t size);
    void operator delete(void* ptr);
};



class Stack
{
public:
    StackPageNode *top;
    StackPageNode *bottom;
    int stackSize;

    Stack();
    // Operations on Deque
    void insertPageAtBottom(void *pageAddr);
    void insertPageAtTop(void *pageAddr);
    void deletePageAtBottom();
    void deletePageAtTop();
    void deletePageIfExists(void *page);
    void* getPageAtbottom();
    void* getPageAtTop();
    int size();
    bool isEmpty();
    void display();
    void destroy();
};

#endif