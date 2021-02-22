#include "misc/Stack.h"



//class Stack
//////////////////////////////////////////////
Stack::Stack(){
    top = bottom = NULL;
    stackSize = 0;
}

void Stack::insertPageAtBottom(void *pageAddr){
    StackPageNode *newStackPageNode = (StackPageNode*) malloc(sizeof(StackPageNode));
    newStackPageNode->pageAddress = pageAddr; 
    newStackPageNode->prev = newStackPageNode->next = NULL;

    if (bottom == NULL)
        top = bottom = newStackPageNode;
    else
    {
        newStackPageNode->prev = bottom;
        bottom->next = newStackPageNode;
        bottom = newStackPageNode;
    }
    stackSize++; 
}

void Stack::insertPageAtTop(void *pageAddr){
    // cout << "Lets insert " << pageAddr << "to the accessStack" << endl;
    ////////////////////////////////////////////
    // Initialization
    StackPageNode *newStackPageNode = (StackPageNode*) malloc(sizeof(StackPageNode));; 
    newStackPageNode->pageAddress = pageAddr; 
    newStackPageNode->prev = newStackPageNode->next = NULL;
    ////////////////////////////////////////////
    if (top == NULL)
        bottom = top = newStackPageNode;
    else
    {
        newStackPageNode->next = top;
        top->prev = newStackPageNode;
        top = newStackPageNode;
    }
    stackSize++;
}

void Stack::deletePageAtBottom()
{
    StackPageNode *temp = bottom;
    bottom = bottom->prev;

    // If only one element was present
    if (bottom == NULL)
        top = NULL;
    else
        bottom->next = NULL;
    delete(temp);

    // Decrements count of elements by 1
    stackSize--;
}

void Stack::deletePageAtTop()
{
    StackPageNode *temp = top;
    top = top->next;

    // If only one element was present
    if (top == NULL)
        bottom = NULL;
    else
        top->prev = NULL;
    free(temp);

    // Decrements count of elements by 1
    stackSize--;
}

void Stack::deletePageIfExists(void *page){
    // b ../src/system/VirtualMem.cc:76
    // If deque is empty then
    // 'Underflow' condition
    if (isEmpty()){
        return;
    }
    if (top->pageAddress == page){
        deletePageAtTop();
    }  
    else if (bottom->pageAddress == page)
    {
        deletePageAtBottom();
    }
    else
    {
        StackPageNode *current = top; 
        while(current != NULL){
            if(current->pageAddress == page){
                current->prev->next = current->next; 
                delete current; 
                this->stackSize--;
                break; 
            }
            current = current->next;
        }
    } 
}

void* Stack::getPageAtbottom(){
    // If deque is empty, then returns
    // garbage value
    if (isEmpty())
        return (void*)-1;
    return bottom->pageAddress;
}


void* Stack::getPageAtTop(){
    // If deque is empty, then returns
    // garbage value
    if (isEmpty()){
        return (void*)-1;
    }
    return top->pageAddress;
}

int Stack::size(){
    return stackSize;
}

bool Stack::isEmpty(){
    return (this->bottom == NULL);
}

void Stack::display(){
    cout << "AccessStack has the following: " << endl; 
    if(isEmpty())
    {
        cout << "Empty !" << endl; 
        cout << "#####################################" << endl;

        return;
    }
    StackPageNode *current = top; 
    while(current != NULL)
    {
        cout << current->pageAddress << endl;
        current = current->next;
    }
    cout << "#####################################" << endl;
}

void Stack::destroy()
{
    top = NULL;
    while (bottom != NULL)
    {
        StackPageNode *temp = bottom;
        bottom = bottom->next;
        free(temp);
    }
    stackSize = 0;
}