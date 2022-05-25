#ifndef BITMAP_H_
#define BITMAP_H_

#include "os_type.h"

class BitMap
{   private:
        int length;
        char *bitmap;
        
    public:
        BitMap();
        void initialize(char *bitmap, const int length);
        bool get(const int index) const;
        void set(const int index, const bool status);
        int allocate(const int count);
        void release(const int index, const int count);
        char *getBitmap();
        int size() const;
        BitMap(const BitMap &) = delete;
        void operator=(const BitMap&) = delete;
};

#endif