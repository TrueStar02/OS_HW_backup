#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "screen.h"

InterruptManager irm;
Screen screen;

extern "C" void setup_kernel()
{   irm.initialize();
    screen.initialize();
    asm_hello_world();
    for(int x=0;x<2000000;x++)
        continue;
    screen.clearScreen();
    irm.enableTimeInterrupt();
    irm.setTimeInterrupt((void *)asm_time_interrupt_handler);
    //asm volatile("sti");
    int x=-12345;
    uint ux=6789;
    myprintf("Test%%:\n"
            "Interger:%d\n"
            "String:%s\n"
            "Unsigned interger:%u\n"
            "Character:%c\n"
            "Hexdecimal:%x\n"
            "Pointer:%p\n"
            "Octal:%o",x,"maimai",ux,'z',ux,&x,ux);
    while(1)
        continue;
}
