#include "disk.h"
#include "x86.h"
#include "screen.h"

void DiskManager::initialize(bool m){isMain=m;}

void DiskManager::message(uint32 start,uint8 num)
{   uint8 to1f3 = start & 0xff;
    uint8 to1f4 = (start >> 8) & 0xff;
    uint8 to1f5 = (start >> 16) & 0xff;
    
    if(isMain)
    {   uint8 to1f6 = ((start >> 24) & 0xf) | 0xe0;
        outb(0x1f2,num);
        outb(0x1f3,to1f3);
        outb(0x1f4,to1f4);
        outb(0x1f5,to1f5);
        outb(0x1f6,to1f6);
    }
}

void DiskManager::read(uint32 start,uint8 num,void * buffer)
{   message(start,num);
    char * c_buf = (char *)buffer;
    uint16 data;
    if(isMain)
    {   outb(0x1f7,0x20);
        for(uint8 i = 0;i < num;i++)
        {   while((inb(0x1f7) & 0x88) != 0x08)
                continue;
            for(int j = 0;j < 512;j++,j++)
            {   
                data = inw(0x1f0);
                c_buf[0] = data & 0xff;
                c_buf[1] = data >> 8;
                c_buf += 2;
            }
        }
    }
}

void DiskManager::write(uint32 start,uint8 num,void * buffer)
{   uint16 data;

    char * c_buf = (char *)buffer;
    message(start,num);
    if(isMain)
    {   outb(0x1f7,0x30);
        for(uint8 i = 0;i < num;i++)
        {   while((inb(0x1f7) & 0x88) != 0x08)
                continue;
            for(int j = 0;j < 512;j++,j++)
            {   data = (uint16)(c_buf[0]) | ((uint16)(c_buf[1]) << 8);
                outw(0x1f0,data);
                c_buf += 2;
            }
        }
    }
}
