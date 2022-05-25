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
SystemService systemService;

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
    
    systemService.initialize();
    systemService.setSystemCall(0, (int)syscall_test);
    int ret = 0;
    ret = asm_system_call(0, 123, 324, 9248, 7, 123);
    myprintf("return value: %d\n", ret);
    while(1)
        continue;
}
