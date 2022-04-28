#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
#include "screen.h"
#include "program.h"
#include "sync.h"

InterruptManager irm;
Screen screen;
ProgramManager programManager;
Semaphore semaphore;
int hamburger;

void son(void *arg) 
{   semaphore.P();
    myprintf("S:Look what I have found!\n");
    myprintf("S:I will eat them\n");
    hamburger-=10;
    semaphore.V();
}
void mother(void *arg) 
{   myprintf("M:I've cooked 10 hamburgers!\n");
    semaphore.P();
    hamburger+=10;
    myprintf("M:I should do some housework\n");
    for(int i=0;i<0xffffff;i++)
        continue;
    myprintf("M:Oh,here are %d hamburgers!\n",hamburger);
    semaphore.V();
}
void first_thread(void *arg)
{   hamburger = 0;
    semaphore.initialize(1);
    screen.clearScreen();
    programManager.executeThread(mother,nullptr,"mother",1);
    programManager.executeThread(son,nullptr,"son",1);
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
   
    int pid=programManager.executeThread(first_thread, nullptr, "first_thread", 1);
    
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
