#include "queue.h"
#include "screen.h"
void Queue::initialize(uint32 * s)
{
    start = s;
    head = 0;
    tail = 0;
    MAX_SIZE = 65536;
}

uint32 Queue::pop()
{
    if(size())
    {
        uint32 temp = start[head];
        //myprintf("%p\n",*(uint32*)0x100020);
        head = (head + 1) % MAX_SIZE;
        //myprintf("%p poped\n",temp);
        return temp;  
    }
    return 0xffffffff;
}

void Queue::push(uint32 item)
{   start[tail] = item;
    tail = (tail + 1) % MAX_SIZE;
    //myprintf("%p pushed\n",item);
    //myprintf("%p\n",start+tail);
    
}

int Queue::size()
{
    return (tail + MAX_SIZE - head) % MAX_SIZE;
}