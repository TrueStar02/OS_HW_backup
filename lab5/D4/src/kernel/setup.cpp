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
    asm volatile("sti");
    while(1)
        continue;
}
