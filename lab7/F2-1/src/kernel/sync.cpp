#include "sync.h"
#include "asm_utils.h"
#include "os_extern.h"

SpinLock::SpinLock()
{
    initialize();
}
void SpinLock::initialize()
{
    bolt = 0;
}
void SpinLock::lock()
{   
    asm_lock(&bolt);
}
void SpinLock::unlock()
{
    bolt = 0;
}
Semaphore::Semaphore()
{
    initialize(0);
}
void Semaphore::initialize(uint32 counter)
{
    this->counter = counter;
    semLock.initialize();
    waiting.initialize();
}
void Semaphore::P()
{
    PCB *cur = nullptr;

    while (true)
    {
        semLock.lock();
        if (counter > 0)
        {
            --counter;
            semLock.unlock();
            return;
        }

        cur = programManager.running;
        waiting.push_back(&(cur->tagInGeneralList));
        cur->status = 3;

        semLock.unlock();
        programManager.schedule();
    }
}
void Semaphore::V()
{
    semLock.lock();
    ++counter;
    if (waiting.size())
    {
        PCB *program = (PCB *)((int)waiting.front()-(int)&(((PCB *)0)->tagInGeneralList));
        waiting.pop_front();
        semLock.unlock();
        programManager.WakeUp(program);
    }
    else
    {
        semLock.unlock();
    }
}