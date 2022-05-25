#ifndef DISK_H_
#define DISK_H_
#include "os_type.h"

class DiskManager
{   
    private:
        bool isMain;
        void message(uint32 start,uint8 num);

    public:
        DiskManager(){}
        void initialize(bool m);
        void read(uint32 start,uint8 num,void * buffer);
        void write(uint32 start,uint8 num,void * buffer);

};
#endif