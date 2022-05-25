#ifndef SYSCALL_H
#define SYSCALL_H


class SystemService
{
public:
    SystemService();
    void initialize();
    // 设置系统调用，index=系统调用号，function=处理第index个系统调用函数的地址
    bool setSystemCall(int index, int function);
};

// 第0个系统调用
int syscall_test(int first, int second, int third, int forth, int fifth);
int syscall_print(int first, int second, int third, int forth, int fifth);
int syscall_readdisk(int first, int second, int third, int forth, int fifth);
int syscall_writedisk(int first, int second, int third, int forth, int fifth);
int syscall_fork(int first, int second, int third, int forth, int fifth);
int syscall_wait(int first, int second, int third, int forth, int fifth);
int syscall_exit(int first, int second, int third, int forth, int fifth);
int syscall_cls(int first, int second, int third, int forth, int fifth);

#endif