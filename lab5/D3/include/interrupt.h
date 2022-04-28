#ifndef INTERRUPT_H_
#define INTERRUPT_H
#include "os_type.h"

class InterruptManager
{   private:
        uint32 * IDT;
    public:
        InterruptManager();
        void initialize();
        void setInterruptDescriptor(uint32 index,uint32 addr,byte DPL);
};

#endif
