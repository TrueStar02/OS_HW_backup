#include "random.h"
#include "os_extern.h"
void srand(uint s)
{
    seed=s;
}
uint rand(void)
{   
    seed=seed*1103515425+12345;
    return (uint)(seed/65536)%32768;
}