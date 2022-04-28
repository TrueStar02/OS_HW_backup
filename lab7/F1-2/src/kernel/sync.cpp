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