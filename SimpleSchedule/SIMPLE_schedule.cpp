#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
using namespace std;
#define MB_input 8  //给定的MB数目
#define MBmen 20000//2000000
#define MBthrou 200000//2000000
const int maxnum = 3000;
const int maxn = 10;
int pathlen[maxnum][maxnum];
int MB_location[maxn];//MB位置
int MB_identifier[maxn];//MB类型，0FW，1Nat，2PROXY
int MB_ca[MB_input][3];//第i个MB，剩余内存，剩余吞吐，使用频率
const int flownum = 2000; //同一时刻最多2K流
int flow[maxnum][4];//随机生成2000个流,源host，目的host，包种类，包个数，包速率
const int packet_kind=7;
int FlowOverhead[packet_kind][2];//每种类型包，在每个middlebox中占用规则，产生处理延时
int chain[packet_kind][10][10];//每种service-chain可能具有的处理序列，每种类型流，组合【0】【1】【2】
int n,line;             // 图的结点数和路径数
void FlowSchedule(){
    for(int i=0; i< MB_input; i++){
        MB_ca[i][0]=MBmen;
        MB_ca[i][1]=MBthrou;
    }
    double lan=0;
    int src,dest,flowkind,packetnum;
    for(int i=0;i< flownum; i++){ //对每条流
        src=flow[i][0];dest=flow[i][1];flowkind=flow[i][2];packetnum=flow[i][3];
        lan+=FlowOverhead[flowkind][1];
    }
    printf("%lf ",lan);
}
void preprocess(){
    srand(time(0));
    double x1=(rand()%100+10)*0.01;
    double x2=(rand()%100+10)*0.01;
    double x3=(rand()%100+10)*0.01;
    double x4=(rand()%100+10)*0.01;
    double x5=1-x4;
    double x6=1-x3;
    FlowOverhead[0][0]=3;FlowOverhead[0][1]=512+457+70+77*x1+77*(1-x1);
    FlowOverhead[1][0]=6;FlowOverhead[1][1]=512+457+70+(167+77)*x2+77*(1-x2);
    FlowOverhead[2][0]=11;FlowOverhead[2][1]=512+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77);
    FlowOverhead[3][0]=9;FlowOverhead[3][1]=512+457+70+77*x1+77*(1-x1)+512+457+70+(167+77)*x2+77*(1-x2);
    FlowOverhead[4][0]=14;FlowOverhead[4][1]=512+457+70+77*x1+77*(1-x1)+512+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77);
    FlowOverhead[5][0]=17;FlowOverhead[5][1]=512+457+70+(167+77)*x2+77*(1-x2)+512+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77);
    FlowOverhead[6][0]=20;FlowOverhead[6][1]=512+457+70+77*x1+77*(1-x1)+512+457+70+(167+77)*x2+77*(1-x2)+512+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77);
}
void FlowRead(){
    FILE*fp1=fopen("FlowCreate.txt","r");  //源host，目的host，包种类，包个数，包速率
    for(int i=0;i<flownum;i++){
        fscanf(fp1,"%d ",&flow[i][0]);
        fscanf(fp1,"%d ",&flow[i][1]);
        fscanf(fp1,"%d ",&flow[i][2]);
        fscanf(fp1,"%d ",&flow[i][3]);
        fscanf(fp1,"%d ",&flow[i][4]);
    }
    fclose(fp1);
}
void input(){
    n=41;line=116;
	int p,q,len;          // 输入p, q两点及其路径长度
    FILE* fp=fopen("topo_2.txt","r");
	for(int i=0;i<line;++i){
        fscanf(fp,"%d ",&p);
        fscanf(fp,"%d ",&q);
        fscanf(fp,"%d ",&len);
        if(len<pathlen[p][q]){       // 有重边
            pathlen[p][q]=len;      // p指向q
            pathlen[q][p]=len;      // q指向p，这样表示无向图
		}
    }
    fclose(fp);
    FILE*fp1= fopen("traditional_MB_location.txt","r");
    for(int i=0; i< MB_input; i++){
        fscanf(fp1,"%d ",&MB_location[i]);
        fscanf(fp1,"%d ",&MB_identifier[i]);
    }
    fclose(fp1);
}
int main() {
    input();
    FlowRead();
    preprocess();
    FlowSchedule();
}
