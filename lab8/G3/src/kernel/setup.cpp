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



extern "C" void setup_kernel()
{   
    asm_hello_world();
    screen.initialize();
    screen.clearScreen();
    irm.initialize();
    irm.enableTimeInterrupt();
    
    irm.setTimeInterrupt((void *)asm_time_interrupt_handler);
    
    programManager.initialize();   
   
    memoryManager.openPageMechanism();
    memoryManager.initialize();
    int va1=memoryManager.allocatePages(0,4);
    int va2=memoryManager.allocatePages(0,2);
    int va3=memoryManager.allocatePages(0,3);
    int va4=memoryManager.allocatePages(0,2);
    memoryManager.releasePages(0,va1,4);
    memoryManager.releasePages(0,va3,3);
    int va5=memoryManager.allocatePages(0,2);
    memoryManager.releasePages(0,va2,2);
    memoryManager.releasePages(0,va4,2);
    memoryManager.releasePages(0,va5,2);
    while(1)
        continue;
}
