#include "interrupt.h"
#include "os_type.h"
#include "asm_utils.h"

InterruptManager::InterruptManager(){initialize();}
void InterruptManager::initialize()
{   IDT = (uint32 *)0x8880;
    asm_lidt(0x8880,2047);
    uint x;
    for (x=0;x<256;x++)
        setInterruptDescriptor(x,(uint32)default_interrupt_handler,0);
}
void InterruptManager::setInterruptDescriptor(uint32 index,uint32 addr,byte DPL)
{   IDT[index<<1]=(0x0020<<16)|(addr&0xffff);
    IDT[index<<1|1]=(addr&0xffff0000)|(1<<15)|(DPL<<13)|(0xe<<8);
}