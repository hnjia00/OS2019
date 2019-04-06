#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/types.h>

int main(int argc, char *argv) {
	/*
	  先打印根节点
	*/
	pid_t p1;
	printf("p1 pid: %d, ppid: %d\n", getpid(),getppid());
	if(p1 == 0){
	/*
		进入进程p1
	*/
		pid_t p3;
		/*
			先创建p3
		*/
		p3 = fork();
		if(p3 == 0)
			/*
				j进入进程p3
			*/
			printf("p3 pid: %d, ppid: %d\n", getpid(),getppid());
		else if(p3 > 0){
			/*
				进入p3的父进程，也就是p1。
				接下来创建进程p2
			*/
			pid_t p2;
			p2 = fork();
			if(p2==0){
				/*
					进入进程p2,接下来先创建p4
				*/
				printf("p2 pid: %d, ppid: %d\n", getpid(),getppid());
				pid_t p4;
				p4 = fork();
				if(p4 == 0){
					/*
						进入进程p4
					*/
					printf("p4 pid: %d, ppid: %d\n", getpid(),getppid());					
				}
				else if(p4 >0){
					/*
						此处位于p4的父进程，即p2,继续创建p5
					*/
					pid_t p5;
					p5 = fork();
					if(p5==0)
						printf("p5 pid: %d, ppid: %d\n", getpid(),getppid());	
				}
			}
		}
	}
	sleep(1);
	
}

