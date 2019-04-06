#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>


//注意：该联合体没有定义在任何系统头文件中，因此得用户自己声明。(centos6.5中/linux/sem.h可以找到)
typedef union _semun {
  int val;	 /* Value for SETVAL */
  struct semid_ds *buf;	 /* Buffer for IPC_STAT, IPC_SET */
  ushort *array;	/* Array for GETALL, SETALL */
}semun;

/*
	int semctl(int semid, int semnum, int cmd, ...);
	功能描述： 
		在指定的信号集或信号集内的某个信号上执行控制操作。
	参数说明：
		semid：信号集的标识符，即是信号表的索引。
		semnum：信号集的索引，用来存取信号集内的某个信号。
		cmd：需要执行的命令:
			SETVAL //根据semun设定信号的值。
			SETALL //将所有semun.array的值设定到信号集中。
			IPC_RMID //立即删除信号集，唤醒所有被阻塞的进程。
*/

// 给指定信号量赋值
void setSemValue(int sem_id, int sem_num,  int val) {
	semun sem_union;
	sem_union.val = val;
	if (semctl(sem_id, sem_num, SETVAL, sem_union) == -1)
	{
		printf("%d %s\n", sem_id, "error - set value of semaphore");
		exit(1);
	}
}

// 给指定信号量集合赋值
void setSemValues(int sem_id, ushort *array) {
	semun sem_union;
	sem_union.array = array;
	if (semctl(sem_id, sizeof(array) - 1, SETALL, sem_union) == -1)
	{
		printf("%d %s\n", sem_id, "error - set values of semaphore");
		exit(1);
	}
}

// //配合semop函数的结构体
// struct sembuf{
//     short sem_num; // 除非使用一组信号量，否则它为0
//     short sem_op;  // 信号量在一次操作中需要改变的数据，通常是两个数，一个是-1，即P（等待）操作，
//                    // 一个是+1，即V（发送信号）操作。
//     short sem_flg; // 通常为SEM_UNDO,使操作系统跟踪信号，
//                    // 并在进程没有释放该信号量而终止时，操作系统释放信号量
// };

void signal(int sem_id,int sem_num){
	struct sembuf semBuf;
	semBuf.sem_num=sem_num;
	semBuf.sem_op=+1;
	semBuf.sem_flg=SEM_UNDO;

	if(semop(sem_id,&semBuf,1)==-1)
	{
		printf("%s\n", "error - signal");
	}
}

void wait(int sem_id,int sem_num){
	struct sembuf semBuf;
	semBuf.sem_num=sem_num;
	semBuf.sem_op=-1;
	semBuf.sem_flg=SEM_UNDO;

	if(semop(sem_id,&semBuf,1)==-1)
	{
		printf("%s\n", "error - wait");
	}
}

/*
	int semget(key_t key, int num_sems, int sem_flags):
	功能描述：
		创建一个新信号量或取得一个已有信号量
	参数说明：
		key:整数值,不相关的进程可以通过它访问一个信号量,它代表程序可能要使用的某个资源
		num_sems:指定需要的信号量数目
		sem_flags:一组标志,当想要当信号量不存在时创建一个新的信号量,可以和值IPC_CREAT做按位或操作
		0666:创建了一个权限为666的信号量,每个人可读和可写
*/

int main(int argc, char *argv[])
{

	pid_t pid1, pid2, pid3;
	int sem1, sem2, sem3;
	sem1 = semget(1000, 2, 0666 | IPC_CREAT);//[0,0]，保证p1最先
	sem2 = semget(1001, 1, 0666 | IPC_CREAT);//[1],保证p2、p3互斥
	sem3 = semget(1002, 2, 0666 | IPC_CREAT);//[0,0]，保证p4最后

	ushort init_arry[2] = {0, 0};
	setSemValues(sem1, init_arry);
	setSemValue(sem2, 0, 1);
	setSemValues(sem3, init_arry);

	pid1 = fork();

	if (pid1 > 0)
	{
		pid2 = fork();
		//p1
		if (pid2 > 0)
		{
			printf("P1:I am P1\n");
			printf("pid,ppid:%d,%d\n",getpid(),getppid() );
			signal(sem1,0);
			signal(sem1,1);
			sleep(1);
		}
		//p4
		if(pid2 == 0)
		{
			wait(sem3, 0);
			wait(sem3, 1);
			printf("P4:I am P4\n");
			printf("pid,ppid:%d,%d\n",getpid(),getppid() );
		}
	}
	//p1
	if (pid1 == 0)
	{
		pid3 = fork();
		//p4
		if (pid3 > 0)
		{
			wait(sem1, 0);
			wait(sem2, 0);
			printf("P2:I am P2\n");
			printf("pid,ppid:%d,%d\n",getpid(),getppid() );
			signal(sem2, 0);
			signal(sem3, 0);
			sleep(1);
			
		}
		//p3
		if (pid3 == 0)
		{
			wait(sem1, 1);
			wait(sem2, 0);
			printf("P3:I am P3\n" );
			printf("pid,ppid:%d,%d\n",getpid(),getppid() );
			signal(sem2, 0);
			signal(sem3, 1);
		}
	}

	return 0;
}
