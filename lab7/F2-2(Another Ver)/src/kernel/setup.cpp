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
char buf[10];
int state;

void Producer(void *arg) 
{   
    const  char * str="0123456789101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354";
    int cnt=0;
    int label;
    while(cnt<10)
    {   
        //semaphore.P();
        label=state;
        //semaphore.V();
        if(label==0)
        {   for(int i=0;i<10;i++)
                buf[i]=str[10*cnt+i];
            cnt++;
            //semaphore.P();
            state=1;
            //semaphore.V();
        }
    }
}
void Consumer(void *arg) 
{   int cnt=0;
    int label;
    while(cnt<10)
    {   
        //semaphore.P();
        label=state;
        //semaphore.V();
        if(label==1)
        {   for(int i=0;i<10;i++)
                myprintf("%c",buf[i]);
            myprintf("\n");
            cnt++;
            //semaphore.P();
            state=0;
            //semaphore.V();
        }
    }
}
void first_thread(void *arg)
{   state=0;
    semaphore.initialize(1);
    screen.clearScreen();
    programManager.executeThread(Producer,nullptr,"producer",1);
    programManager.executeThread(Consumer,nullptr,"consumer",1);
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
