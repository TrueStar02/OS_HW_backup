#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "os_extern.h"
#include "screen.h"
#include "program.h"
#include "sync.h"
#include "tools.h"
#include "memory.h"

InterruptManager irm;
Screen screen;
ProgramManager programManager;
uint seed;
MemoryManager memoryManager;
DiskManager maindisk;

int test[256];
int test2[256];
extern "C" void setup_kernel()
{   
    asm_hello_world();
    screen.initialize();
    screen.clearScreen();
    irm.initialize();
    irm.enableTimeInterrupt();
    
    irm.setTimeInterrupt((void *)asm_time_interrupt_handler);
    maindisk.initialize(1);
    programManager.initialize();   
   
    memoryManager.openPageMechanism();
    memoryManager.initialize();
    
    int va1=memoryManager.allocatePages(0,255);
    ((uint32 *)va1)[0] = 8;
    ((uint32 *)va1)[4096] = 18;
    myprintf("value1 = %u\n",((uint32 *)va1)[0]);
    myprintf("value2 = %u\n",((uint32 *)va1)[4096]);
    //myprintf("value1 = %u\n",*(uint32 *) va1);
    int va2=memoryManager.allocatePages(0,15);
    
    myprintf("value1 = %u\n",((uint32 *)va1)[0]);
    myprintf("value2 = %u\n",((uint32 *)va1)[4096]);
    memoryManager.releasePages(0,va1,255);
    memoryManager.releasePages(0,va2,15);
    while(1)
        continue;
}
