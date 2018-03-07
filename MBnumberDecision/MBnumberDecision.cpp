#include <iostream>
#include <vector>
#include <stack>
#include<stdio.h>
using namespace std;
const int maxn=10;
const int maxnum = 3000;
const int maxint = 999999;
int dist[maxnum];     // 表示当前点到源点的最短路径长度
int c[maxnum][maxnum];   // 记录图的两点间路径长度
int n,line;             // 图的结点数和路径数
int num_shortest[maxnum]={0};//i点到j点的最短路径条数，i*N+j
int shortest_path[maxnum][maxn][maxn]={0};//i到j点，第一条路径，第一个，二个...点
int len_shortest[maxnum]={0};//i点到j点最短路径跳数（长度）
int vt,ut;//起点，终点
int num_node[maxnum]={0};//记录每个结点在最短路径中出现的频率
int node_shortest[maxnum]={-1}; //记录该路径通过哪个节点处理，默认为-1
int path_shortest[maxnum]={-1};//记录两点间通过哪条最短路径完成处理，默认为-1
int lan_all=0;                     //每种方案下总跳数
int op_lan;
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
	if (u==v) {
	    len_shortest[vt*n+ut]=len;
	    int j=0;
	    shortest_path[vt*n+ut][num_shortest[vt*n+ut]][j++]=v;
		for(int k=len-1;k>=0;k--){
			shortest_path[vt*n+ut][num_shortest[vt*n+ut]][j++]=sta[k];
		}
//		cout<<v;
		num_shortest[vt*n+ut]++;
	    return ;
	}
	sta[len]=u;
	for(int i=0;i<prev[u].size();++i) {
		if(i>0){
			for(int j=len-1;j>=0;--j) {
//				cout<<"->"<<sta[j];
			}
//			cout<<endl;
		}
		searchPath(prev,v,prev[u][i],sta,len+1);
//		cout<<"->"<<u;
	}
}
void MbSelect(int m,int MBnum){  //m为关键节点编号
	//只有一个MB
	op_lan=0;//如果每一个switch上都有一个MB，消耗的最少跳数
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			if(i!=j){
				op_lan+=len_shortest[i*n+j];
		//		cout<<"i "<<i<<"j "<<j<<" "<<op_lan<<endl;
				int num2=num_shortest[i*n+j];  //等价多路径跳数
				int numl=len_shortest[i*n+j]+1;  //跳数，结点数目还需+1
				for(int k2=0;k2<num2;k2++){
					if(node_shortest[i*n+j]==-1){
						for(int k1=0;k1<numl;k1++){
							if(m==shortest_path[i*n+j][k2][k1]){
								node_shortest[i*n+j]=m;  //该点可使用第k2条最短距离,并通过m结点处理
								path_shortest[i*n+j]=k2; //
								lan_all+=len_shortest[i*n+j];
								break;
							}
						}
					}
					else break;
				}
			}
		}
	}
	cout<<op_lan<<" "<<lan_all<<endl;
	if(MBnum==1){
		for(int i=0;i<n;i++){
			for(int j=0;j<n;j++){
				if(i!=j){
					if(node_shortest[i*n+j]==-1){  //该路径需要其他关键点处理
						lan_all+=len_shortest[i*n+m]+len_shortest[m*n+j];
						node_shortest[i*n+j]=m;
					}
				}
			}
		}
	}
	else{
		for(int ii=0;ii<maxnum;ii++){
			num_node[ii]=0;
		}
		for(int i=0;i<n;i++){
			for(int j=0;j<n;j++){
				if(i!=j){               //重新计算最大结点，分配
					if(node_shortest[i*n+j]==-1){  //该路径需要其他关键点处理
						int nums=num_shortest[i*n+j];
						int numl=len_shortest[i*n+j]+1;  //跳数，结点数目还需+1
						for(int k1=0;k1<numl-1;k1++){
							num_node[shortest_path[i*n+j][0][k1]]++;
						}
					}
				}
			}
		}
		//计算最大结点
		int maxf=0;
		for(int i=0;i<n;i++){
			if(num_node[maxf]<num_node[i]){
				maxf=i;                          //找出频率最大结点
			}
		}
		MbSelect(maxf,MBnum-1);
	}
	cout<<op_lan<<" "<<lan_all<<endl;
}
void nodefrequency(int n){ //计算每个结点在最短路径中出现的频率
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			int nums=num_shortest[i*n+j];
			int numl=len_shortest[i*n+j]+1;  //跳数，结点数目还需+1
			for(int k1=0;k1<numl-1;k1++){
				num_node[shortest_path[i*n+j][0][k1]]++;
				if(shortest_path[i*n+j][0][k1]==shortest_path[i*n+j][0][k1+1]){
					len_shortest[i*n+j]=k1;
					break;
				}
			}
		}
	}
	cout<<endl<<endl;
//	for(int i=0;i<n;i++){
//		cout<<i<<" "<<num_node[i]<<endl;
//	}
	//获得频率最大的结点
	int maxf=0;
	for(int i=0;i<n;i++){
		if(num_node[maxf]<num_node[i]){
			maxf=i;                          //找出频率最大结点
		}
	}
	MbSelect(maxf,40);
}
int main() {
	for(int i=0;i<maxnum;i++){
		node_shortest[i]=-1;
		path_shortest[i]=-1;
	}
    vector<int> prev[maxnum];     // 记录当前点的前一个结点
	n=41;line=116;
	int p,q,len;          // 输入p, q两点及其路径长度
 	for(int i=0;i<n;++i)
		for(int j=0;j<n;++j)
			c[i][j]=maxint;
	FILE* fp=fopen("topo_2.txt","r");
	for(int i=0;i<line;++i){
        fscanf(fp,"%d",&p);
        fscanf(fp,"%d",&q);
        fscanf(fp,"%d",&len);
        if(len<c[p][q]){       // 有重边
            c[p][q]=len;      // p指向q
            c[q][p]=len;      // q指向p，这样表示无向图
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
			Dijkstra(n,i,dist,prev,c);
			if(i!=j){
//    			cout << i<<"点到"<<j<<"的最短路径长度: " << dist[j] << endl;
// 				cout << i<<"点到"<<j<<"的路径为: "<<endl;
				int sta[maxnum];
				searchPath(prev,i,j,sta,0);
    		}
//    		cout<<endl;
//			int nums=num_shortest[i*n+j];
//			int numl=len_shortest[i*n+j]+1;  //跳数，结点数目还需+1
//			for(int k=0;k<nums;k++){
//				for(int k1=0;k1<numl;k1++){
//					cout<<shortest_path[i*n+j][k][k1]<<" ";
//				}
//														 //i到j路线起点shortest_path[i*n+j][0][0]
//				                                        //终点shortest_path[i*n+j][0][len_shortest[i*n+j]
//														//跳数len_shortest[i*n+j]
//														//等价多路径数目num_shortest[i*n+j]
//			}
//			cout<<endl;
    	}
    }
    //计算结点频率
    cout<<endl;
    nodefrequency(n);
}

