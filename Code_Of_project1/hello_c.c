#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
        write(1,"hello world!\r\n",14);

        return 0;
}