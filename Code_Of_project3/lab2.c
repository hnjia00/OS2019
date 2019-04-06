#include<fcntl.h>
#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <sched.h>

int ticketCount=1000;//剩余票数
int temp;
sem_t *empty = NULL;//记录剩余空间
sem_t *full = NULL;//记录剩余票数
sem_t *mutex = NULL;//互斥信号量
int N_sell=100000;//售票数
int N_return=99000;//退票数
int pthread_yield(void);

//售票
void *Psell(){
	int i=0;
	for(;i<N_sell;i++){
		sem_wait(full);
		//等待互斥信号
		sem_wait(mutex);
		//售票进程关键代码
		temp=ticketCount;
		//pthread_yield();
		temp=temp-1;
		//pthread_yield();
		ticketCount=temp;
		printf("[Sell - %d]\n",i+1);
		sem_post(empty);
		sem_post(mutex);
	}
	//sleep(10);
	printf("[Sell end: total sell:%d]\n",i);
}

//退票进程
void *Preturn(){
	int i=0;
	for(;i<N_return;i++){
		sem_wait(empty);
		//等待互斥信号
		sem_wait(mutex);
		//退票进程关键代码
		temp=ticketCount;
		pthread_yield();
		temp=temp+1;
		pthread_yield();
		ticketCount=temp;
		printf("{Return + %d}\n",i+1);
		sem_post(full);
		sem_post(mutex);
	}
	//sleep(2);
	printf("{Return end: total return:%d}\n",i);
}

int main(int argc,char *argv[]){
	pthread_t p1,p2;
	full=sem_open("full",O_CREAT,0666,1000);//记录剩余票数
	empty=sem_open("empty",O_CREAT,0666,0);//记录剩余空间
	mutex=sem_open("mutex",O_CREAT,0666,1);

	printf("[Begin]:ticketCount is :%d\n",ticketCount);

	pthread_create(&p1,NULL,Psell,NULL);
	pthread_create(&p2,NULL,Preturn,NULL);
	pthread_join(p2,NULL);
	pthread_join(p1,NULL);

	printf("[End]:ticketCount is :%d\n",ticketCount);
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);
	sem_unlink("empty");
	sem_unlink("mutex");
	sem_unlink("full");
	return 0;
}

