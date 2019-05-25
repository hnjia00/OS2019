//
//  main.cpp
//  页面置换
//
//  Created by 贾浩男 on 2019/5/16.
//  Copyright © 2019年 贾浩男. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <ctime>
#include <list>

#define N 32
#define M 3
#define K 2
using namespace std;

//记录请求页号有关信息
typedef struct {
    int request;
    bool lack=false;//lack = true为缺页
    int modify=0;//记录访问页号是否被修改
}req;
vector<req> re;//数组记录，re[i]代表第i个时段进来的页号+是否缺页中断

//记录物理块情况,M个块、N个周期
int bar[M][N]={-1};
int access[M]={0};//记录M个块所存页号的访问位
int modify[M]={0};//记录M个块所存页号的修改位

//计算缺页率
double cal_lost(int n){
    int lost=0;
    for(int i=0;i<n;i++){
        if(re[i].lack==true){
            lost++;
            //re[i].lack=false;
        }
    }
    return (1.0)*lost/n;
}

//改进clock: m个块，n个周期序列
/*
 1类(A =0, M = 0)：表示该页面最近既未被访问，又未被修改，是最佳淘汰页。
 2类(A =0, M = 1)：表示该页面最近未被访问，但已被修改，并不是很好的淘汰页。
 */
double new_clock(int m,int n){
    memset(bar, -1, sizeof(bar));
    //初始化前0〜m-1个序列
    //i: 周期
    for(int i=0;i<m;i++){
        //j: 块
        for(int j=0;j<=i;j++){
            bar[j][i] = re[j].request;
            access[j] = 1;//被访问
            modify[j] = re[j].modify;//被修改过
        }
    }
    int p=0;//充当指针
    
    //装载第i: m〜n-1个序列
    for(int i=m;i<n;i++){
        //对j：0〜m块，首先把第i-1时段各块的情况拷贝给第i的时段
        for(int j=0;j<m;j++){
            bar[j][i] = bar[j][i-1];
        }
        //记录第i个请求
        int now_req=re[i].request;
        int now_modify=re[i].modify;
        //判断现在内存中有无该请求页号
        bool has=false;
        for(int j=0;j<m;j++){
            if(bar[p][i]==now_req){
                has=true;
                access[p]=1;
                modify[p]=now_modify;
                p = (p+1)%m;
                break;
            }
            p = (p+1)%m;
        }
        //有的话直接下一个
        if(has) continue;
        
        // 没有的话要考虑把谁替换掉
        // 先记录缺页
        re[i].lack=true;
        
        
        do{
            //先找第一类 access=0 && modify==0
            bool first=false;
            //从指针所指位置开始
            for(int j=0;j<m;j++){
                if(access[p]==0 && modify[p]==0){
                    bar[p][i] = now_req;//找到则替换
                    access[p] = 1;//被访问
                    modify[p] = now_modify;
                    
                    p = (p+1)%m;
                    first=true;
                    break;
                }
                p = (p+1)%m;
            }
            if(first){
                break;
            }
            
            //第一类失败
            //再第二类 access=0 && modify==1
            bool second=false;
            for(int j=0;j<m;j++){
                
                if(access[p]==0 && modify[p]==1){
                    bar[p][i] = now_req;//找到则替换
                    access[p] = 1;//被访问
                    modify[p] = now_modify;
                    
                    p = (p+1)%m;
                    second=true;
                    break;
                }
                else{
                    access[p]=0;//访问位均改为0
                    p = (p+1)%m;
                }
            }
            if(second){
                break;
            }
            
            //第二类失败
            //指针返回开始位置
            p=0;
            //将所有的访问位复0
            for(int j=0;j<m;j++){
                access[j]=0;
            }
        }while(1);
    }
    return cal_lost(n);
}

//最佳置换: m个块，n个周期序列
double OPT(int m,int n){
    memset(bar, -1, sizeof(bar));
    //初始化前0〜m-1个序列
    //i: 周期
    for(int i=0;i<m;i++){
        //j: 块
        for(int j=0;j<=i;j++){
            bar[j][i] = re[j].request;
        }
    }
    //装载第i: m〜n-1个序列
    for(int i=m;i<n;i++){
        //对j：0〜m块，首先把第i-1时段各块的情况拷贝给第i的时段
        for(int j=0;j<m;j++){
            bar[j][i] = bar[j][i-1];
        }
        //记录第i个请求
        int now_req=re[i].request;
        //判断现在内存中有无该请求页号
        bool has=false;
        for(int j=0;j<m;j++){
            if(bar[j][i]==now_req){
                has=true;
                break;
            }
        }
        //有的话直接下一个
        if(has) continue;
        
        // 没有的话要考虑把谁替换掉
        // 先记录缺页
        re[i].lack=true;
        
        // 考察序列范围：第i+1个到第n-1个
        // 对m个块中个字各自装载的页号考察
        bool find=false;
        int far_no=-1,far_m=-1;//记录最远出现的页号和其所在块
        for(int j=0;j<m;j++){
            int temp_no=bar[j][i];
            int temp_m=-1;
            int k=i+1;
            for(;k<n;k++){
                //在未来的请求序列中找到了temp_no
                if(re[k].request==temp_no){
                    temp_m=k;
                    find=true;
                    break;
                }
            }
            //记录距离最大的
            if(temp_m>far_m){
                far_m=temp_m;
                far_no=j;
            }
            //未来请求中没有，则肯定替换这个
            if(k==n){
                find=false;
                bar[j][i]=now_req;
                break;
            }
        }
        //找到了一个未来中最大的
        if(find){
            bar[far_no][i]=now_req;
        }
    }
    
    return cal_lost(n);
}

//先进先出: m个块，n个周期序列
double FIFO(int m,int n){
    memset(bar, -1, sizeof(bar));
    //初始化前0〜m-1个序列
    //i: 周期
    for(int i=0;i<m;i++){
        //j: 块
        for(int j=0;j<=i;j++){
            bar[j][i] = re[j].request;
        }
    }
    //装载第i: m〜n-1个序列
    for(int i=m;i<n;i++){
        //对j：0〜m块，首先把第i-1时段各块的情况拷贝给第i的时段
        for(int j=0;j<m;j++){
            bar[j][i] = bar[j][i-1];
        }
        //记录第i个请求
        int now_req=re[i].request;
        //判断现在内存中有无该请求页号
        bool has=false;
        for(int j=0;j<m;j++){
            if(bar[j][i]==now_req){
                has=true;
                break;
            }
        }
        //有的话直接下一个
        if(has) continue;
        
        // 没有的话要考虑把谁替换掉
        // 先记录缺页
        re[i].lack=true;
        
        // 考察序列范围：第i+1个到第n-1个
        // 对m个块中个字各自装载的页号考察
        // 需要向前检查bar[j][i]，即bar[j]目前内存中的页号装载了几个周期
        int long_m=-1;//记录待了最久的bar[j]的次数
        int long_j=-1;//记录待了最久的bar[j]的下标j
        for(int j=0;j<m;j++){
            int temp_m=0;
            int temp_no=bar[j][i];
            for(int k=i-1;k>0;k--){
                if(temp_no==bar[j][k])
                    temp_m++;
                else
                    break;
            }
            if(temp_m>long_m){
                long_m=temp_m;
                long_j=j;
            }
        }
        
        //替换
        bar[long_j][i]=now_req;
    }
    return cal_lost(n);
}

double LRU(int m,int n){
    memset(bar, -1, sizeof(bar));
    //初始化前0〜m-1个序列
    //i: 周期
    for(int i=0;i<m;i++){
        //j: 块
        for(int j=0;j<=i;j++){
            bar[j][i] = re[j].request;
        }
    }
    //装载第i: m〜n-1个序列
    for(int i=m;i<n;i++){
        //对j：0〜m块，首先把第i-1时段各块的情况拷贝给第i的时段
        for(int j=0;j<m;j++){
            bar[j][i] = bar[j][i-1];
        }
        //记录第i个请求
        int now_req=re[i].request;
        //判断现在内存中有无该请求页号
        bool has=false;
        for(int j=0;j<m;j++){
            if(bar[j][i]==now_req){
                has=true;
                break;
            }
        }
        //有的话直接下一个
        if(has) continue;
        
        // 没有的话要考虑把谁替换掉
        // 先记录缺页
        re[i].lack=true;
        
        // 考察序列范围：第i-1个到第0个
        // 对m个块中个字各自装载的页号考察
        bool find=false;
        int far_no=-1,far_m=0xff;//记录最远出现的页号和其所在块
        for(int j=0;j<m;j++){
            int temp_no=bar[j][i];
            int temp_m=-1;
            int k=i-1;
            for(;k>0;k--){
                //在过去的请求序列中找到了temp_no
                if(re[k].request==temp_no){
                    temp_m=k;
                    find=true;
                    break;
                }
            }
            //记录距离最大的
            if(temp_m<far_m){
                far_m=temp_m;
                far_no=j;
            }
        }
        //找到了一个过去中最远出现的
        if(find){
            bar[far_no][i]=now_req;
        }
    }
    return cal_lost(n);
}

//PBA:基于先进先出: m个块，n个周期序列，k个空闲块
// 空闲页面链表
typedef struct {
    int what;//块内容
    int which;//块号
}idle_bar;
list<idle_bar> idle;
// 物理块
typedef struct {
    int flag=0;//存在位
    int No=-1;//块号
    int time=-1;//time时进入块，用于辅助先进先出判断
}NODE;
NODE node[M+K];
double PBA(int m,int n,int k){
    
    //初始化前0〜m-1个序列
    //i: 周期
    printf("预设:\n");
    for(int i=0;i<m;i++){
        node[i].No=re[i].request;
        node[i].flag=1;
        node[i].time=i;
    }
    for(int i=0;i<m;i++)
        printf("%d ",node[i].No);
    cout<<endl;
    //idle装入k个空闲物理块
    for(int i=0;i<k;i++){
        idle_bar temp;
        temp.what=-1;
        temp.which=m+i;
        idle.push_back(temp);
    }
    //装载第i: m〜n-1个序列
    for(int i=m;i<n;i++){
        int now_req=re[i].request;
        
        //先在物理块中寻找有没有
        int has=-2;
        for(int j=0;j<m+k;j++){
            //存在位为0，说明是挂在idle上的块
            if(node[j].flag==0&&node[j].No==now_req){
                //has记录在哪个块中
                has=j;
                break;
            }
            if(node[j].flag==1&&node[j].No==now_req){
                has=-1;
                break;
            }
        }
        //在内存中且在驻留集中
        if(has==-1){
            printf("[%d]: 在驻留集中\n",now_req);
            continue;
        }
        //在内存中但不在驻留集中
        if(has>-1){
            printf("[%d]: 在空闲链表上\n",now_req);
            //要把其拿回到驻留集中
            for(list<idle_bar>::iterator it=idle.begin();it!=idle.end();it++){
                if(it->what==now_req){
                    //fifo替换准则
                    int mintime=n,minjj=-1;
                    for(int jj=0;jj<m+k;jj++){
                        if(node[jj].flag==0)//只能替换存在的
                            continue;
                        if(node[jj].time<mintime){
                            mintime=node[jj].time;
                            minjj=jj;
                        }
                    }
                    node[minjj].flag=0;
                    node[has].flag=1;//更新为存在
                    node[has].time=i;//更新时间
                    //在链表上删除掉，并把替换下来的minjj放入idle中
                    idle.erase(it);
                    idle_bar temp;
                    temp.what=node[minjj].No;
                    temp.which=minjj;
                    idle.push_back(temp);
                    break;
                }
            }
            for(int ii=0;ii<m+k;ii++)
                if(node[ii].flag==1)printf("%d ",node[ii].No);
            cout<<endl;
            continue;
        }
        
        //如果不在物理块(内存)中
        if(has==-2){
            printf("[%d]: 不在物理块中\n",now_req);
            re[i].lack=true;//缺页
            //基于fifo替换
            int mintime=n,minjj=-1;
            for(int jj=0;jj<m+k;jj++){
                if(node[jj].flag==0)//只能替换存在的
                    continue;
                if(node[jj].time<mintime){
                    mintime=node[jj].time;
                    minjj=jj;
                }
            }
            //决定了要被替换的为node[minjj]
            
            //先看idle中的空闲块没用满的情况
            //则新来的直接进空闲块
            int flag1=-1;
            for(list<idle_bar>::iterator it=idle.begin();it!=idle.end();it++){
                //空闲块
                if(it->what==-1){
                    node[it->which].flag=1;
                    node[it->which].No=now_req;
                    node[it->which].time=i;
                    idle.erase(it);//idle链表上删去这个空闲块
                    
                    idle_bar temp;
                    temp.what=node[minjj].No;
                    temp.which=minjj;
                    idle.push_back(temp);//被替换下来的进idle
                    node[minjj].flag=0;
                    flag1=1;
                    break;
                }
            }
            if(flag1==1){
                for(int ii=0;ii<m+k;ii++)
                    if(node[ii].flag==1)printf("%d ",node[ii].No);
                cout<<endl;
                continue;//空闲替换成功
            }
            
            //idle中已经没有空闲块的情况
            //需要替换链首块的内容
            //将now_req放入此时idle链首的块里，原来链首的内容被彻底移出内存块
            //根据fifo提出新的块放入idle中
            int head=idle.begin()->which;
            node[head].flag=1;
            node[head].time=i;
            node[head].No=now_req;
            idle.pop_front();//移出队首
            
            idle_bar temp;
            temp.what=node[minjj].No;
            temp.which=minjj;
            idle.push_back(temp);//被替换下来的进idle
            node[minjj].flag=0;
            
            for(int ii=0;ii<m+k;ii++)
                if(node[ii].flag==1)printf("%d ",node[ii].No);
            cout<<endl;
        }
    }
    
    return cal_lost(n);
}




// 页面访问序列随机生成
/*
 虚拟内存大小：16位=64K
 工作集的起始位置p，
 工作集中包含的页数e，
 工作集移动率m（每处理m个页面访问则将起始位置p +1），
 以及一个范围在0和1之间的值t；
 */
void init()
{
    int p = rand() % 64;
    int m = 8, e = 4;
    int i, j;
    double t;
    t = rand() % 10 / 10.0;
    for (i = 0; i < 4; i++)
    {
        //生成m个取值范围在p和p + e间的随机数，并记录到页面访问序列串中；
        for (j = i * m; j < (i + 1) *m; j++)
        {
            req temp;
            //access[j] = (p + rand() % e) % 64;
            temp.request = (p + rand() % e) % 64;
            temp.modify = rand()%2;//被随机修改，只针对clock算法
            re.push_back(temp);
        }
        //生成一个随机数r，0 ≤ r ≤ 1；
        double r = (rand() % 10) / 10.0;
        
        //如果r < t，则为p生成一个新值，否则p = (p + 1) mod N
        if (r<t)
        {
            p=rand()%64;
        }
        else
        {
            p=(p+1)%64;
        }
    }
}



int main(int argc, const char * argv[]) {
    srand ( (unsigned) time (0)); //用时间做种，每次产生随机数不一样
    
    //m个块，n个请求
    
    freopen("/Users/jiahaonan/Desktop/in.txt", "r", stdin);
    int m,n;
    cin>>m>>n;
    
    for(int i=0;i<n;i++){
        req temp;
        cin>>temp.request;
        temp.modify = rand()%2;//被随机修改，只针对clock算法
        re.push_back(temp);
    }
    
    // 输出请求序列
    cout<<"页号序列："<<endl;
    for(int i=0;i<n;i++){
        printf("%2d%1s",re[i].request," ");
    }
    cout<<endl;
    
    /*
    init();
    int m=3, n=32;
    */
    /**************************************************************/
    double pba=PBA(m, n, 2);
    cout<<endl;
    cout<<"PBA:"<<pba<<endl;
    cout<<"OPT:"<<OPT(m, n)<<endl;
    cout<<"FIFO:"<<FIFO(m, n)<<endl;
    cout<<"LRU:"<<LRU(m, n)<<endl;
    cout<<"new_clock:"<<new_clock(m, n)<<endl;
    
    /**************************************************************/
    

     /*
     cout<<"\n修改序列："<<endl;
     for(int i=0;i<n;i++){
     printf("%2d%1s",re[i].modify," ");
     }
     
     cout<<endl<<endl;
     cout<<"块信息："<<endl;
     // 输出各个块的信息
     for(int i=0;i<m;i++){
     for(int j=0;j<n;j++){
     if(bar[i][j]==-1)
     printf("%3s"," ");
     else
     printf("%2d%1s",bar[i][j]," ");
     
     }
     cout<<endl;
     }
     
     // 输出缺页信息
     for(int i=0;i<n;i++){
     if(re[i].lack)
     printf("%3s"," X ");
     else
     printf("%3s"," ");
     }
     cout<<endl;*/
    
    
    return 0;
}
