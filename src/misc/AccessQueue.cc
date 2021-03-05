#include "misc/AccessQueue.h"

void Queue::enqueue(void *pageAddress)
{
    if (front == -1)
        front = 0;

    rear = (rear + 1) % MAX_PAGES_IN_ACCESS_STACK;
    accessedPages[rear] = pageAddress;
}

void Queue::putElementAtRear(void *element){
    if(checkEmpty()){
        cout << "error: now Elements in Queue, can't put one at rear" << endl;
        return;
    }

    int currentIndex = front;

    while(currentIndex != ((rear+1) % MAX_PAGES_IN_ACCESS_STACK)){
        if(accessedPages[currentIndex] == element){
            this->enqueue(element);
            void *remAddress1 = accessedPages[front];
            void *remAddress2 = accessedPages[(front + 1 )% MAX_PAGES_IN_ACCESS_STACK];
            front = (front + 1) % MAX_PAGES_IN_ACCESS_STACK;
            int i = front;
            while(i!=((currentIndex + 1)%MAX_PAGES_IN_ACCESS_STACK)){
                accessedPages[i] = remAddress1;
                remAddress1 = remAddress2;
                remAddress2 = accessedPages[(i + 1) % MAX_PAGES_IN_ACCESS_STACK];

                i = (i + 1)%MAX_PAGES_IN_ACCESS_STACK;
            }
            return;
        }

        currentIndex = (currentIndex + 1) % MAX_PAGES_IN_ACCESS_STACK;
    }

    cout << "error: element to put at rear not found" << endl;
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
        cout << accessedPages[i] << endl;;
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
