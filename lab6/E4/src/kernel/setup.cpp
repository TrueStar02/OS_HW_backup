#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "screen.h"
#include "program.h"

InterruptManager irm;
Screen screen;
ProgramManager programManager;
void test_thread(void *arg) 
{   myprintf("pid %d name \"%s\": start!\n", programManager.running->pid, programManager.running->name);
    for(int x=0;x<1000000000;x++)
        continue;
    myprintf("pid %d name \"%s\": end!\n", programManager.running->pid, programManager.running->name); 
}
void run_thread(void *arg)
{   myprintf("pid %d name \"%s\": start!\n", programManager.running->pid, programManager.running->name);
    programManager.executeThread(test_thread, nullptr, "first thread", 2);
    programManager.executeThread(test_thread, nullptr, "second thread", 2);
    programManager.executeThread(test_thread, nullptr, "third thread", 1);
    programManager.executeThread(test_thread, nullptr, "forth thread", 0);
    programManager.executeThread(test_thread, nullptr, "fifth thread", 1);
    myprintf("pid %d name \"%s\": end!\n", programManager.running->pid, programManager.running->name); 
}
void init_thread(void *arg)
{   myprintf("pid %d name \"%s\": start!\n", programManager.running->pid, programManager.running->name);
    if (!programManager.running->pid)
        programManager.executeThread(run_thread, nullptr, "run thread", 0);
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
    
    programManager.initialize();   
   
    int pid=programManager.executeThread(init_thread, nullptr, "init thread", 3);
    
    if (pid == -1)
    {
        myprintf("can not execute thread\n");
        while(1)
            continue;
    }
    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = (PCB *)((int)item-(int)&(((PCB *)0)->tagInGeneralList));
    firstThread->status = 2;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);
    while(1)
        continue;
}
