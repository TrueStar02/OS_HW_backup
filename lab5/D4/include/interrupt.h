#ifndef INTERRUPT_H_
#define INTERRUPT_H
#include "os_type.h"

class InterruptManager
{   private:
        uint32 * IDT;
        void init8259A();
    public:
        InterruptManager();
        void initialize();
        void setInterruptDescriptor(uint32 index,uint32 addr,byte DPL);
        void enableTimeInterrupt();
        void disableTimeInterrupt();
        void setTimeInterrupt(void *handler);
};

#endif
