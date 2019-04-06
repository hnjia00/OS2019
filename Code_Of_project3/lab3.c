#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

#define N 5
int buf[N];
//信号量
sem_t *empty=NULL,*full=NULL;
//T1、T2分别代表输入输出的测试规模
int T1=15,T2=15;
//in：输入指针，out：输出指针
int in=0,out=0;

//输入线程
void *producer() {
	int nextp;
	while(T1--){
		//sem_wait(empty);
		printf("%s\n","input: ");
		scanf("%d",&nextp);
		buf[in]=nextp;
		in = (in+1)%N;
		//sem_post(full);
	}
	printf("%s\n", "[input]:over");
	return NULL;
}

//输出线程
void *consumer() {
	int nextc;
	while(T2--){
		//sem_wait(full);
		nextc=buf[out];
		sleep(1);
		printf("output:%d\n", nextc);
		out = (out+1)%N;
		//sem_post(empty);
	}
	printf("%s\n", "[output]:over");
	return NULL;
}

int main(int argc, char *argv[])
{

	 pthread_t p1, p2;

	 empty = sem_open("empty",O_CREAT,0666,5);
	 full = sem_open("full",O_CREAT,0666,0);

	 pthread_create(&p1, NULL, producer, NULL);
	 pthread_create(&p2, NULL, consumer, NULL);
	 pthread_join(p1, NULL);
	 pthread_join(p2, NULL);

	 sem_close(empty);
	 sem_close(full);

	 sem_unlink("empty");
	 sem_unlink("full");
	 return 0;
}	
