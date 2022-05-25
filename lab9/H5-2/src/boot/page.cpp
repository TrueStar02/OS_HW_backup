

extern "C" void openPageMechanism()
{
    int *directory = (int *)0x200000;
    int *page = (int *)(0x201000);


    for(int i = 0;i < 1024;i++)
    {   
        page[i] = 0;
        directory[i] = 0;
    }

    int address = 0;
    for (int i = 0; i < 512; ++i)
    {
        page[i] = address | 0x7;
        address += 0x1000;
    }

    directory[0] = ((int)page) | 0x07;
    directory[768] = directory[0];
    directory[1023] = ((int)directory) | 0x7;
    
}