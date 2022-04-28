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
{   uint32 key = 1;
    do
    {   asm_atomic_exchange(&key, &bolt);
    }   while (key);
}
void SpinLock::unlock()
{
    bolt = 0;
}