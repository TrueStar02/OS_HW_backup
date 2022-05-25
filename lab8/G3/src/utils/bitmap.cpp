#include "bitmap.h"
#include "tools.h"

BitMap::BitMap(){}

void BitMap::initialize(char *bitm, const int len)
{
    bitmap = bitm;
    length = len;

    int bytes = ceil((double)length / 8);

    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

int BitMap::allocate(const int count)
{
    if (count == 0)
        return -1;

    int head = -1;
    int bestlen = 999999999;
    int curlen,curhead = -1;
    for(int i = 0;i < length;i++)
    {   if (get(i))
        {   if(curhead != -1)
            {
                curlen = i - curhead;
                if(curlen >= count)
                {   if(curlen < bestlen)
                    {   bestlen = curlen;
                        head = curhead;
                    }
                    if(curlen == count)
                        break;
                }
            }    
            curhead = -1;

        }
        else
        {
            if(curhead == -1)
                curhead = i;
        }
    }

    if(curhead != -1)
    {
        curlen = length - curhead;
        if(curlen >= count)
        {   if(curlen < bestlen)
            {   bestlen = curlen;
                head = curhead;
            }
        }
    }
    
    if(head != -1)
    {
        for(int i = head;i < count + head;i++)
            set(i,1);
    }

    return head;
}

void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}