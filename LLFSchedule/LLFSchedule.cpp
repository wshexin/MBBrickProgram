#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<malloc.h>
#include<time.h>
#include <vector>
#include <stack>
#define MB_input 8   //给定的MB数目
#define MBmen 20000//2000000
#define MBthrou 20000000//2000000
using namespace std;
const int maxn=10;
const int maxnum = 3000;
const int maxint = 999999;
int MB[MB_input]={24,7,37,15,29,32,12,21};//存储mb
int MB_load[MB_input]={0};
int MB_utilization[MB_input]={0};
//CI CH CPy RI RP RPy FD FF
const int packet_kind=7;
int MBkind[packet_kind+1];//每种类型包是否有直接可以使用的MB,没有可以使用的为100
int packet_MB[packet_kind+1][MB_input];  //记录该类型的flow被哪些编号的mb处理
int packet_MB_len[MB_input];//记录该类型的流被几个mb处理，数目
int MB_ca[MB_input][3];//第i个MB，剩余内存，剩余吞吐,[i][2] packet_kind
int bandwidth = 300000;//每条链路带a宽相同
int vt,ut;//起点，终点
//FW 1 0 0 0 0 0 1 1
//NAT 1 0 0 1 1 0 0 1
//PROXY 1 1 1 1 0 1 0 1
//FW+NAT 2 0 0 1 1 0 1 1
//FW+PROXY 2 1 1 1 0 1 1 2
//NAT+PROXY 2 1 1 2 1 1 0 2
//FW+NAT+PROXY 3 1 1 2 1 1 1 3
const int flownum = 4000; //同一时刻最多2K流
int flow[maxnum][4];//随机生成2000个流,源host，目的host，包种类，包个数，包速率
int PacketRate = bandwidth/flownum;
int PacketSize = 84;
int shortest_pathlength[maxnum][maxnum];//记录任意两点间最短距离
int node_shortest[maxnum]={-1}; //记录该路径通过哪个节点处理，默认为-1
int path_shortest[maxnum]={-1};//记录两点间通过哪条最短路径完成处理，默认为-1
int dist[maxnum];     // 表示当前点到源点的最短路径长度
int pathlen[maxnum][maxnum];   // 记录图的两点间路径长度
int n,line;             // 图的结点数和路径数
int num_path[maxnum]={0};//i点到j点的all路径条数，i*N+j
int whole_path[maxnum][maxn][maxn]={0};//i到j点，第一条路径，第一个，二个...点
int len_longest[maxnum]={0};//i点到j点最chang路径跳数（长度）
double ProcessLatency;
int rule[packet_kind]={3,6,11,9,14,17,20};
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
int FLowProcessLatency(int i, int state){
    srand(time(0));
    double x1=(rand()%100+10)*0.01;
    double x2=(rand()%100+10)*0.01;
    double x3=(rand()%100+10)*0.01;
    double x4=(rand()%100+10)*0.01;
    double x5=1-x4;
    double x6=1-x3;
    if(state ==0){  //create module
        if(flow[i][2]==1){
            ProcessLatency =532+512+457+70+77*x1+77*(1-x1);
        }
        else if(flow[i][2]==2){
            ProcessLatency =532+512+457+70+(167+77)*x2+77*(1-x2);
        }
        else if(flow[i][2]==3){
            ProcessLatency =532+512+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77);
        }
        else if(flow[i][2]==4){
            ProcessLatency =532+512+457+70+77*x1+(70+(167+77)*x2+77*(1-x2))*(1-x1);
        }
        else if(flow[i][2]==5){
            ProcessLatency =532+512+457+70+77*x1+(x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
        else if(flow[i][2]==6){
            ProcessLatency =
            532+512+457+70+(167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+(1-x3)*(70+77);
        }
        else if(flow[i][2]==7){
            ProcessLatency =
            532+512+457+70+77*x1+((167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
    }
    else if(state == 1){  //use directly
        if(flow[i][2]==1){
            ProcessLatency =457+70+77*x1+77*(1-x1);
        }
        else if(flow[i][2]==2){
            ProcessLatency =457+70+(167+77)*x2+77*(1-x2);
        }
        else if(flow[i][2]==3){
            ProcessLatency =457+70+457+x3*(70+77*x4+(207+167+77)*x5)+(1-x3)*(70+77);
        }
        else if(flow[i][2]==4){
            ProcessLatency =457+70+77*x1+(70+(167+77)*x2+77*(1-x2))*(1-x1);
        }
        else if(flow[i][2]==5){
            ProcessLatency =512+457+70+77*x1+(x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
        else if(flow[i][2]==6){
            ProcessLatency =
            457+70+(167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+(1-x3)*(70+77);
        }
        else if(flow[i][2]==7){
            ProcessLatency =
            457+70+77*x1+((167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
    }
    else  {//insert rule
        if(flow[i][2]==1){
            ProcessLatency =270+457+70+77*x1+77*(1-x1);
        }
        else if(flow[i][2]==2){
            ProcessLatency =270+457+70+(167+77)*x2+77*(1-x2);
        }
        else if(flow[i][2]==3){
            ProcessLatency =270+457+70+457+x3*(70+77*x4+(207+167+77)*x5)+(1-x3)*(70+77);
        }
        else if(flow[i][2]==4){
            ProcessLatency =270+457+70+77*x1+(70+(167+77)*x2+77*(1-x2))*(1-x1);
        }
        else if(flow[i][2]==5){
            ProcessLatency =270+512+457+70+77*x1+(x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
        else if(flow[i][2]==6){
            ProcessLatency =
            270+457+70+(167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+(1-x3)*(70+77);
        }
        else if(flow[i][2]==7){
            ProcessLatency =
            270+457+70+77*x1+((167+77)*x2+77*(1-x2)+457+x3*(70+77*x4+(207+167+77)*x5)+x6*(70+77))*(1-x1);
        }
    }
    return ProcessLatency;
}
void FlowSchedule(){
    //第一时刻，空的MB，最小代价流，匹配MB和flow
    for(int i=0;i<packet_kind+1; i++){
        MBkind[i] = 100;
    }
    for(int i=0; i< MB_input; i++){
        MB_ca[i][0]=MBmen;
        MB_ca[i][1]=MBthrou;
    }
    double lan=0;
    int src,dest,flowkind,packetnum;
    for(int i=0;i< flownum; i++){ //对每条流
        src=flow[i][0];dest=flow[i][1];flowkind=flow[i][2];packetnum=flow[i][3];
        int shortlen=shortest_pathlength[src][MB[0]];
        int nummb = 0;//分配给流的MB编号
        if(MBkind[flowkind]==100){
            for(int j=1; j< MB_input; j++){
                //选择最近的MB，最短距离最小
                if(MB_ca[j][0]> rule[flowkind] ){  //内存足够，存储规则
                    if(shortest_pathlength[src][MB[j]] < shortlen){
                        shortlen = shortest_pathlength[src][MB[j]];
                        nummb = j;
                    }
                }
            }
            MB_load[nummb]++;
            MB_utilization[nummb]+=rule[flowkind];
                  //      printf("%d %d %d\n", j,MB_load[j],MB_utilization[j]);
            packet_MB[flowkind][packet_MB_len[flowkind]++]=nummb;  //记录该种类型流，被哪个mb处理
            //记录每个MB处理的不同流类型个数
            MB_ca[nummb][0]-= rule[flowkind];
            MB_ca[nummb][1]-= packetnum;
            MB_ca[nummb][2]= flowkind;
            MBkind[flowkind]= 1;
            lan += FLowProcessLatency(i,0);
            cout<<i<<endl;
        }
        else {  //有MB处理了这种flow
            int flag=0;//标记该流是否被处理
            for(int j=0; j<packet_MB_len[flowkind]; j++){
                if(MB_ca[nummb][1] >= packetnum){
                    int mbnumber = packet_MB[flowkind][j];
                    nummb = mbnumber;
                    MB_ca[nummb][1]-= packetnum;
                    lan += FLowProcessLatency(i,1);
                    flag=1;
                    MB_load[nummb]++;
                    MB_utilization[nummb]+=rule[flowkind];
                 //   printf("%d %d %d\n", nummb,MB_load[nummb],MB_utilization[nummb]);
                    cout<<i<<endl;
                    break;
                }
            }
            if(flag==0){  //该流未被已经存在的MB处理掉
                //考虑选择MB添加规则, 添加在资源剩余多的mb上
                for(int j=0; j<MB_input; j++){
                    if(MB_ca[j][1] >= packetnum&&MB_ca[j][0] >= rule[flowkind]){
                        packet_MB[flowkind][packet_MB_len[flowkind]++]=j;
                        int mbnumber = j;
                        nummb =mbnumber;
                        MB_ca[nummb][1]-= packetnum;
                        MB_ca[nummb][0]-= rule[flowkind];
                        MB_load[nummb]++;
                        MB_utilization[nummb]+=rule[flowkind];
                        //printf("%d %d %d\n", nummb,MB_load[nummb],MB_utilization[nummb]);
                        lan += FLowProcessLatency(i,2);
                        cout<<i<<endl;
                        break;
                    }
                }
            }
        }
    }
    for(int i=0;i<packet_kind+1; i++){
        printf("%d ",MBkind[i]);
    }
    printf("%lf\n",lan);
    for(int i=0;i<MB_input;i++){
        printf("%d %d %d\n", i,MB_load[i],MB_utilization[i]);
    }
}
void Dijkstra(int n,int v,int*dist,vector<int>*prev,int c[maxnum][maxnum]){
	bool s[maxnum];    // 判断是否已存入该点到S集合中
	for(int i=0;i<n;++i){
		dist[i]=c[v][i];
		s[i]=0;     // 初始都未用过该点
		if(dist[i]<maxint)
			prev[i].push_back(v);
	}
	dist[v]=0;
	s[v]=1;
 	for(int i=1;i<n;++i){
		int tmp=maxint;
		int u=v;
		for(int j=0;j<n;++j)
			if((!s[j])&&dist[j]<tmp){
				u=j;              // u保存当前邻接点中距离最小的点的号码
				tmp=dist[j];
			}
		s[u]=1;    // 表示u点已存入S集合中
		for(int j=0;j<n;++j)
			if((!s[j])&&c[u][j]<maxint){
				int newdist=dist[u]+c[u][j];
				if(newdist<=dist[j]){
					if(newdist<dist[j]) {
					  prev[j].clear();
					  dist[j]=newdist;
					}
					prev[j].push_back(u);
				}
			}
	}
}
 // 查找从源点v到终点u的路径，并输出
void searchPath(vector<int>*prev,int v,int u,int sta[],int len) {
    int js=0;
	if (u==v) {
	    len_longest[vt*n+ut]=len;
	    int js=0;
	    whole_path[vt*n+ut][num_path[vt*n+ut]][js++]=v;
		for(int k=len-1;k>=0;k--){
			whole_path[vt*n+ut][num_path[vt*n+ut]][js++]=sta[k];
		}
	//	cout<<v;
		num_path[vt*n+ut]++;
	    return ;
	}
	sta[len]=u;
	for(int i=0;i<prev[u].size();++i) {
		if(i>0){
			for(int j=len-1;j>=0;--j) {
		//		cout<<"->"<<sta[j];
				whole_path[vt*n+ut][num_path[vt*n+ut]][js++]=sta[j];
			}
			cout<<endl;
		}
		searchPath(prev,v,prev[u][i],sta,len+1);
		//cout<<"->"<<u;
		whole_path[vt*n+ut][num_path[vt*n+ut]][js++]=u;
	}
}
void input(){
    for(int i=0;i<maxnum;i++){
		node_shortest[i]=-1;
		path_shortest[i]=-1;
	}
    vector<int> prev[maxnum];     // 记录当前点的前一个结点
	int n=41;line=116;
	int p,q,len,bd;          // 输入p, q两点及其路径长度
 	for(int i=0;i<n;++i){
		for(int j=0;j<n;++j){
			pathlen[i][j]=maxint;
        }
    }
    FILE* fp=fopen("topo_2.txt","r");
	for(int i=0;i<line;++i){
        fscanf(fp,"%d",&p);
        fscanf(fp,"%d",&q);
        fscanf(fp,"%d",&len);
        if(len<pathlen[p][q]){       // 有重边
            pathlen[p][q]=len;      // p指向q
            pathlen[q][p]=len;      // q指向p，这样表示无向图
		}
    }
    fclose(fp);
	for(int i=0;i<n;++i)
		dist[i]=maxint;
 	for(int i=0;i<n;i++){
    	for(int j=0;j<n;j++){
			vector<int> prev[maxnum];     // 记录当前点的前一个结点
			vt=i;
			ut=j;
			Dijkstra(n,i,dist,prev,pathlen);
			if(i!=j){
    		//	cout << i<<"点到"<<j<<"的最短路径长度: " << dist[j] << endl;
    			shortest_pathlength[i][j]=dist[j];
 			//	cout << i<<"点到"<<j<<"的路径为: "<<endl;
				int sta[maxnum];
				searchPath(prev,i,j,sta,0);
    		}
//    		cout<<endl;
			int nums=num_path[i*n+j];
			int numl=len_longest[i*n+j]+1;  //跳数，结点数目还需+1
/*			for(int k=0;k<nums;k++){
				for(int k1=0;k1<numl;k1++){   //跳数len_longest[i*n+j]    //all径数目num_path[i*n+j]
					cout<<whole_path[i*n+j][k][k1]<<" ";    //i到j路线起点whole_path[i*n+j][0][0]
				}                                        //终点whole_path[i*n+j][0][len_longest[i*n+j]
			}
			cout<<endl;
*/    	}
    }
}
int main() {
    int a,b;//parameter,a*fre+b*degree
    input();
    FlowRead();
    FlowSchedule();
}
