#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int n=41;//
int bandwidth = 20000;
#define flownum 4000 //同一时刻最多2K流
int flow[flownum+10][5];//src,dest,packetnum,pathkind
int main(){
    FILE*fp=fopen("hflow.txt","w");
    srand(time(0));
	for(int i=0;i<flownum;i++){  //随机生成2000个流,源host，目的host，包个数，包路径
		flow[i][0]=rand()%rand()%n;
		flow[i][1]=rand()%rand()%n;
		flow[i][2]=rand()%100+10;
		flow[i][3]=rand()%3;
		fprintf(fp,"%d ", flow[i][0]);
		fprintf(fp,"%d ", flow[i][1]);
		fprintf(fp,"%d ", flow[i][2]);
		fprintf(fp,"%d ", flow[i][3]);
		fprintf(fp,"\n");
	}
	fclose(fp);
	return 0;
}
