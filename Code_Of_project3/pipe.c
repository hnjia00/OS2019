/*
 * Filename: pipe.c
 * 只有一个进程，读写管道文件
 */
 
#include <stdio.h>
#include <unistd.h>     //for pipe()
#include <string.h>     //for memset()
#include <stdlib.h>     //for exit()

int main()
{
    ////定义文件描述符数组
    int fd[2];
    char buf[20];
    if(-1 == pipe(fd))
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    //fd[1]描述只写
    write(fd[1], "hello,world", 12);
    memset(buf, '\0', sizeof(buf));
    //fd[0]描述只读
    read(fd[0], buf, 12);
    printf("The message is: %s\n", buf);

    return 0;
}