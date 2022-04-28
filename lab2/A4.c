#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char buf[4096]; 
int i;
int main(int argc,char ** argv)
{   if(argc<3)
    {   puts("Too few arguments!");
        exit(1);
    }
    FILE * fin=fopen(argv[1],"rb");
    if(fin==NULL)
    {   puts("Cannot find the file!");
        exit(1);
    }
    FILE * fout=fopen(argv[2],"wb");    
    while(i=fread(buf,1,4096,fin))
        fwrite(buf,1,i,fout);
    fclose(fin);
    fclose(fout);
    if(argc>=4&&!strcmp(argv[3],"-p"))
    {   fin=fopen(argv[1],"r");
        while(fgets(buf,4096,fin))
        {   i=strlen(buf);
            __asm__ __volatile__ (
                    ".code32\n\t"
                    "pusha\n\t"
                    "mov  $4,%%eax\n\t"
                    "mov  $1,%%ebx\n\t"
                    "int $0x80\n\t"
                    "popa"
                    ::"c"(buf),"d"(i)
                    );
        }
    }
    return 0;
}