#include "screen.h"
#include "asm_utils.h"
#include "os_type.h"
#include "x86.h"

Screen::Screen(){initialize();}
void Screen::initialize(){base=(uint8 *)0xb8000;}
void Screen::print(uint x,uint y,uint8 c,uint8 color)
{   if(x>=25||y>=80)
        return;
    uint pos=x*80+y;
    base[pos<<1]=c;
    base[pos<<1|1]=color;
}
void Screen::print(uint8 c,uint8 color)
{   uint pos=getCursor();
    base[pos<<1]=c;
    base[pos<<1|1]=color;
    pos++;
    if(pos==2000)
    {   rollUp();
        pos=1920;
    }
    moveCursor(pos);
}
void Screen::moveCursor(uint position)
{   if(position>=2000)
        return;
    uint8 temp;

    temp=(position>>8)&0xff;
    outb(0x3d4,0x0e);
    outb(0x3d5,temp);

    temp=position&0xff;
    outb(0x3d4,0x0f);
    outb(0x3d5,temp);
}
void Screen::moveCursor(uint x,uint y)
{   if(x>=25||y>=80)
        return;
    moveCursor(x*80+y);
}
uint Screen::getCursor()
{
    uint pos=0;
    uint8 temp=0;
   
    outb(0x3d4,0x0e);
    temp=inb(0x3d5);
    pos=((uint)temp)<<8;

    outb(0x3d4,0x0f);
    temp=inb(0x3d5);
    pos=pos|(uint)temp;

    return pos;
}
void Screen::rollUp()
{   uint x;
    for(x=80;x<2000;x++)
    base[(x-80)<<1]=base[x<<1];
    base[(x-80)<<1|1]=base[x<<1|1];
    for(x=1920;x<2000;x++)
    {   base[x<<1]=' ';
        base[x<<1|1]=0x07;
    }

}
void Screen::clearScreen()
{   uint x;
    for(x=0;x<2000;x++)
    {   base[x<<1]=' ';
        base[x<<1|1]=0x07;
    }
    moveCursor(0);
}