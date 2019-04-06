#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
	pid_t pid;
	pid = fork();

	//父进程: pid>0
	if(pid > 0) while(1);
	
	
	//子进程： pid=0
	else if(pid == 0){
		int ret;
		ret = execlp("vi","",NULL);

		if (ret == -1){
			perror ("execl");
			printf("excel error\n");
		}
	}

	else if(pid == -1){
		perror("fork");
		printf("fork error\n");
	}  

}
