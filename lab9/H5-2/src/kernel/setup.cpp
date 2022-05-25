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
TSS tss;

int fork(){return asm_system_call(4);}
int wait(int * retval){return asm_system_call(5,(int)retval);}
void exit(int ret){asm_system_call(6,ret);}

void first_process()
{
    int pid = fork();
    int retval;

    if (pid)
    {   
        while ((pid = wait(&retval)) != -1)
        {
            myprintf("wait for a child process, pid: %d, return value: %d\n", pid, retval);
        }

        myprintf("all child process exit, programs: %d\n", programManager.allPrograms.size());
                
        while(1)
            continue;
    }
    else
    {   pid = fork();
        if (pid)
        {
            myprintf("exit, pid: %d\n", programManager.running->pid);
            exit(114514);
        }
        else
        {
            for(int i = 0;i < 0xfffffff;i++)
                continue;
            myprintf("exit, pid: %d\n", programManager.running->pid);
            exit(1919810);
        }
    }
}

void first_thread(void *arg)
{

    myprintf("start process\n");
    programManager.executeProcess((const char *)first_process, 1);
    while(1)
        continue;
}
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
   
    memoryManager.initialize();
    
    systemService.initialize();
    systemService.setSystemCall(0, (int)syscall_test);
    systemService.setSystemCall(1, (int)syscall_print);
    systemService.setSystemCall(2, (int)syscall_readdisk);
    systemService.setSystemCall(3, (int)syscall_writedisk);
    systemService.setSystemCall(4, (int)syscall_fork);
    systemService.setSystemCall(5, (int)syscall_wait);
    systemService.setSystemCall(6, (int)syscall_exit);
    systemService.setSystemCall(7, (int)syscall_cls);
    int ret = 0;
    ret = asm_system_call(0, 123, 324, 9248, 7, 123);
    myprintf("return value: %d\n", ret);
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        myprintf("can not execute thread\n");
        while(1)
            continue;
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = (PCB *)((int)item-(int)&(((PCB *)0)->tagInGeneralList));;
    firstThread->status = 2;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);
    while(1)
        continue;
}
