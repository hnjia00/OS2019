#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/types.h>

int main(int argc, char *argv) {
	/*
	  先打印根节点
	*/
	pid_t p1;
	
	if(p1 == 0){
	/*
		进入进程p1
	*/
		int p1pid,p1ppid;
		p1pid = getpid();
		p1ppid = getppid();
		//printf("p1 pid: %d, ppid: %d\n", getpid(),getppid());
		pid_t p3;
		/*
			先创建p3
		*/
		p3 = fork();
		if(p3 == 0){
			/*
				j进入进程p3
			*/
			int i;
			for(i=0;i<10;i++){
				printf("p3 pid: %d, ppid: %d\n", getpid(),getppid());
				sleep(1);
			}
			return 0;
		}
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
				pid_t p4;
				p4 = fork();
				if(p4 == 0){
					/*
						进入进程p4
					*/
					int i;
					for(i=0;i<10;i++){
						printf("p4 pid: %d, ppid: %d\n", getpid(),getppid());		
						sleep(1);
					}
					return 0;
								
				}
				else if(p4 >0){
					/*
						此处位于p4的父进程，即p2,继续创建p5
					*/
					pid_t p5;
					p5 = fork();
					if(p5==0){
					//p5
						int i;
						for(i=0;i<10;i++){
							printf("p5 pid: %d, ppid: %d\n", getpid(),getppid());
							sleep(1);	
						}
						return 0;
					}

					else{
					//p2
						int i;
						for(i=0;i<10;i++){
							//通过exit()终止p2
							if(i==5)
								exit(1);

							//段错误
							//if(i==5)
							//{
							//	int *p=NULL;
							//	*p=0;
							//}
							printf("p2 pid: %d, ppid: %d\n", getpid(),getppid());
							sleep(1);
						}
						return 0;
					}
				}
					
			}
		}

		int i;
		for(i=0;i<10;i++){
			printf("p1 pid: %d, ppid: %d\n", getpid(),getppid());
			sleep(1);
		}
		return 0;		
	}
	sleep(1);
	
}

