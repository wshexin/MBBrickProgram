#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int n=41;//
int bandwidth = 20000;
#define flownum 2000 //同一时刻最多2K流
int sentrate= bandwidth/flownum;
int packet_kind=7;
int flow[flownum+10][5];//src,dest,packetnum,mbkind,sentrate
int main(){
    FILE*fp=fopen("FlowCreate.txt","w");
    srand(time(0));
	for(int i=0;i<flownum;i++){  //随机生成2000个流,源host，目的host，包种类，包个数，包速率
		flow[i][0]=rand()%rand()%n;
		flow[i][1]=rand()%rand()%n;
		flow[i][2]=rand()%packet_kind+1;
		flow[i][3]=rand()%100+10;
		flow[i][4]=rand()%sentrate+1;
		fprintf(fp,"%d ", flow[i][0]);
		fprintf(fp,"%d ", flow[i][1]);
		fprintf(fp,"%d ", flow[i][2]);
		fprintf(fp,"%d ", flow[i][3]);
		fprintf(fp,"%d ", flow[i][4]);
		fprintf(fp,"\n");
	}
	fclose(fp);
	return 0;
}
