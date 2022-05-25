#ifndef SCREEN_H_
#define SCREEN_H_
#include "os_type.h"

class Screen
{   private:
        uint8 * base;
    public:
        Screen();
        void initialize();
        void print(uint x,uint y,uint8 c,uint8 color);
        void print(uint8 c,uint8 color=0x07);
        void print(const char * str,uint8 color=0x07);
        void moveCursor(uint position);
        void moveCursor(uint x,uint y);
        uint getCursor();
        void rollUp();
        void clearScreen();
};

int myprintf(const char * fmt,...);
int myprintk(const char * fmt,...);
#endif
