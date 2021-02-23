#include "misc/AccessQueue.h"

void Queue::enqueue(void *pageAddress)
{
    if (checkFull())
    {
        dequeue();
    }

    if (front == -1)
        front = 0;

    rear = (rear + 1) % MAX_PAGES_IN_ACCESS_STACK;
    accessedPages[rear] = pageAddress;
}

void* Queue::dequeue()
{
    void* pageAddress; 
    if (checkEmpty())
    {

        return NULL;
    }
    else
    {
        pageAddress = accessedPages[front];
        if (front == rear)
        {
            front = rear = -1;
        }
        else
        {
            front = (front + 1) % MAX_PAGES_IN_ACCESS_STACK;
        }
    }
    cout << pageAddress << " has been dequeued" << endl; 
    return pageAddress; 
}

void Queue::print()
{
    int i;
    if (checkEmpty())
        return; 
    else
    {
        for (i = front; i != rear; i = (i + 1) % MAX_PAGES_IN_ACCESS_STACK)
        {
            cout << accessedPages[i] << endl;
        }
        cout << accessedPages[i];
    }
}

bool Queue::checkFull()
{
    if ((front == rear + 1) || (front == 0 && rear == MAX_PAGES_IN_ACCESS_STACK - 1))
    {
        return true;
    }
    return false;
}

bool Queue::checkEmpty()
{
    if (front == -1)
    {
        return true;
    }
    return false;
}
