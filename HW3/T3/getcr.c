#include <linux/kernel.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");
static int myModule_init(void)
{   long a,b,c,d;
    printk("Getting infomation\r\n");
    
    asm volatile("mov %%cr0,%0":"=a"(a));
    asm volatile("mov %%cr2,%0":"=a"(b));
    asm volatile("mov %%cr3,%0":"=a"(c));
    asm volatile("mov %%cr4,%0":"=a"(d));

    printk("cr0=%016lx\n",a);
    printk("cr2=%016lx\n",b);
    printk("cr3=%016lx\n",c);
    printk("cr4=%016lx\n",d);
    return 0;
}

static void myModule_exit(void)
{
    printk("Module Uninstalled.\r\n");
}
module_init(myModule_init);
module_exit(myModule_exit);
