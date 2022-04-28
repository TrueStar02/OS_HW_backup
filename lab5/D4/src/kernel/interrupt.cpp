#include "interrupt.h"
#include "os_type.h"
#include "asm_utils.h"
#include "x86.h"
#include "screen.h"

extern Screen screen;

InterruptManager::InterruptManager(){initialize();}
void InterruptManager::initialize()
{   IDT = (uint32 *)0x8880;
    asm_lidt(0x8880,2047);
    uint x;
    for (x=0;x<256;x++)
        setInterruptDescriptor(x,(uint32)default_interrupt_handler,0);
    init8259A();
}
void InterruptManager::setInterruptDescriptor(uint32 index,uint32 addr,byte DPL)
{   IDT[index<<1]=(0x0020<<16)|(addr&0xffff);
    IDT[index<<1|1]=(addr&0xffff0000)|(1<<15)|(DPL<<13)|(0xe<<8);
}
void InterruptManager::enableTimeInterrupt()
{
    uint8 value=0xfe&inb(0x21);
    outb(0x21,value);
}
void InterruptManager::disableTimeInterrupt()
{
    uint8 value=0x01|inb(0x21);
    outb(0x21,value);
}
void InterruptManager::setTimeInterrupt(void *handler)
{
    setInterruptDescriptor(0x20,(uint32)handler,0);
}
void InterruptManager::init8259A()
{
    outb(0x20,0x11);
    outb(0xa0,0x11);
    outb(0x21,0x20);
    outb(0xa1,0x28);
    outb(0x21,4);
    outb(0xa1,2);
    outb(0x21,1);
    outb(0xa1,1);
    outb(0x21,0xfb);
    outb(0xa1,0xff);
}

int times=0;
uint8 color=0x09;
uint8 str[100]="20337171 BianRunhua";
bool flag;
extern "C" void c_time_interrupt_handler()
{   flag^=1;
    if(flag)
    {
        times++;
        if(times%100==0)
            color++;
        if(color==0x10)
            color=0x09;
        uint8 tmp=str[0];
        uint x;
        for(x=0;x<99;x++)
            str[x]=str[x+1];
        str[99]=tmp;
        for(x=0;x<80;x++)
            screen.print(0,x,str[x+20],color);
    }
    
}