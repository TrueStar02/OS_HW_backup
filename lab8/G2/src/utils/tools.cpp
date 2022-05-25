#include "tools.h"
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
void memset(void * start,uint8 val,uint32 len)
{   uint8 *head=(uint8 *)start;
    for(uint32 i=0;i<len;i++)
    {
        head[i] = val;
    }

}

int floor(double x){return x;}

int ceil(double x)
{   double delta = x - floor(x);
    if (delta < 1e-8)
        return x;
    else
        return x+1;
}