#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char* msg = "Hello World!\n\r";
    int len = 14;
    int result = 0;
 
    asm volatile("mov %2, %%edx;\n\r" /*传入参数：要显示的字符串长度*/
             "mov %1, %%ecx;\n\r" /*传入参赛：文件描述符（stdout）*/
             "mov $1, %%ebx;\n\r" /*传入参数：要显示的字符串*/
             "mov $4, %%eax;\n\r" /*系统调用号：4 sys_write*/
             "int  $0x80" /*触发系统调用中断*/
             :"=m"(result) /*输出部分：本例并未使用*/
             :"m"(msg),"r"(len)  /*输入部分：绑定字符串和字符串长度变量*/
             :"%eax"); 
        
    return 0;
}


