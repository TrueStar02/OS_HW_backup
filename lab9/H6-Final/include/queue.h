#ifndef QUEUE_H_
#define QUEUE_H_
#include "os_type.h"

class Queue
{      
    private:
        uint32 * start;
        uint32 MAX_SIZE;
        uint32 head;
        uint32 tail;
    public:
        Queue(){}
        void initialize(uint32 * s,uint32 max);
        uint32 pop();
        void push(uint32 item);
        int size();

};
#endif