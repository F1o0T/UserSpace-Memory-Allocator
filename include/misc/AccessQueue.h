#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
using namespace std;
#define MAX_PAGES_IN_ACCESS_STACK 100

class Queue
{
public:
    void* accessedPages[MAX_PAGES_IN_ACCESS_STACK];
    int front = - 1; 
    int rear = - 1;
    bool checkFull();

    void putElementAtRear(void * element);

    bool checkEmpty();

    void enqueue(void* pageAddress);

    void* dequeue();

    void print();
};

#endif
