#include "asm_utils.h"
#include "interrupt.h"
#include "os_type.h"
InterruptManager irm;

extern "C" void setup_kernel()
{   irm.initialize();
    asm_hello_world();
    for(int x=0;x<2000000;x++)
        continue;
    int a=1/0;
    while(1)
        continue;
}
