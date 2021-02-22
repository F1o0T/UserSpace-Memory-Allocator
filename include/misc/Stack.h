#ifndef STACK_H
#define STACK_H

#include <iostream>

using namespace std;


struct StackPageNode {
    void *pageAddress;
    StackPageNode *prev, *next;
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