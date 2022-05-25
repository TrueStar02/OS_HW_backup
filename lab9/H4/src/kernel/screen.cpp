#include "screen.h"
#include "asm_utils.h"
#include "os_type.h"
#include "x86.h"
#include "os_extern.h"
Screen::Screen(){initialize();}
void Screen::initialize(){base=(uint8 *)0xc00b8000;}
void Screen::print(uint x,uint y,uint8 c,uint8 color)
{   if(x>=25||y>=80)
        return;
    uint pos=x*80+y;
    base[pos<<1]=c;
    base[pos<<1|1]=color;
}
void Screen::print(uint8 c,uint8 color)
{   uint pos=getCursor();
    if(c=='\n')
    {
        pos=80*(pos/80+1);
    }
    else
    {
        base[pos<<1]=c;
        base[pos<<1|1]=color;
        pos++;
    }
    
    if(pos==2000)
    {   rollUp();
        pos=1920;
    }
    moveCursor(pos);
}
void Screen::print(const char * str,uint8 color)
{   int x;
    for(x=0;str[x];x++)
        print(str[x],color);

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

char buffer[4097];
int cnt;
int bi;
int isKernel;
void flush_buf(void)
{   if(bi)
    {   buffer[bi]=0;
        if(isKernel)
            screen.print(buffer);
        else
            asm_system_call(1,(int)buffer);
    }
    bi=0;
}
const char trans[17]="0123456789abcdef";
void push_buf(char c)
{   buffer[bi]=c;
    bi++;
    cnt++;
    if(bi==4096)
        flush_buf();
}
void printNum(uint n,int base,int sgn)
{   int num[11],i=0,x;
    if(sgn)
    {   if(n>0x7fffffffu)
        {   push_buf('-');
            n=~n+1;
        }
    }
    while(n)
    {   num[i]=n%base;
        n/=base;
        i++;
    }
    if(i)
    {   for(x=i-1;x>=0;x--)
            push_buf(trans[num[x]]);     
    }
    else
        push_buf('0');

}
void printStr(const char * str)
{   int x;
    for(x=0;str[x];x++)
        push_buf(str[x]);

}
int myprint(const char * fmt,uint * ap)
{   int x;
    cnt=0;
    bi=0;
    uint k;
    for(x=0;fmt[x];x++)
    {   if(fmt[x]=='%')
        {   x++;
            switch(fmt[x])
            {   case '%':
                    push_buf(fmt[x]);
                    break;
                case 'd':
                case 'i':
                    printNum(*ap,10,1);
                    ap++;
                    break;
                case 'u':
                    printNum(*ap,10,0);
                    ap++;
                    break;
                case 'p':
                    push_buf('0');
                    push_buf('x');
                    k = (1 << 28);
                    while(k > 3)
                    {   if(*ap < k)
                        {   push_buf('0');
                            k >>= 4;
                        }
                        else
                            break;
                    }
                case 'x':
                    printNum(*ap,16,0);
                    ap++;
                    break;
                case 'o':
                    printNum(*ap,8,0);
                    ap++;
                    break;
                case 'c':
                    push_buf(*((char *)ap));
                    ap++;
                    break;
                case 's':
                    printStr((const char *)*ap);
                    ap++;
                    break;
            }
        }
        else
            push_buf(fmt[x]);
    }
    flush_buf();
    return cnt;
}

int myprintk(const char * fmt,...)
{   isKernel = 1;
    uint * ap=(uint *)(&fmt + 1);
    return myprint(fmt,ap);
}

int myprintf(const char * fmt,...)
{   isKernel = 0;
    uint * ap=(uint *)(&fmt + 1);
    return myprint(fmt,ap);
}