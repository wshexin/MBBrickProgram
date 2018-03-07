//min{max(len)}
#include <iostream>
#include<stdio.h>
#include <vector>
#include <stack>
#define FLOWNUM  200
using namespace std;
int rest[99999999];
const int MB_input=10;//输入mb数目
const int maxn=5000;
const int mm=300;
const int maxnum = 3000;
const int maxint = 999999;
int MBcapacity[20]={3000000};//MB吞吐量限制    hflow[i][2], flow packets number
//hflow[i][7] flow bandwidth limit  bandwidth[i][j];
int MB_result[maxnum]={0};//计算每种选定MB情况下的，流大小×路径长度开销
int hflow_3[2000][4];//src,dest,packets
int s;//计算排列组合后一共有多少情况
int bandwidth = 200000;//每条链路带宽相同
int flag_hflow[maxnum]={0};//记录该条历史流量是否被分配,初始为0，未分配
int dist[maxnum];     // 表示当前点到源点的最短路径长度
int shortest_pathlength[maxnum][mm];//记录任意两点间最短距离
int pathlen[maxnum][maxnum];   // 记录图的两点间路径长度
int n,line;             // 图的结点数和路径数
int num_path[maxnum]={0};//i点到j点的all路径条数，i*N+j
int whole_path[maxn][mm][mm]={0};//i到j点，第一条路径，第一个，二个...点
int len_longest[maxnum]={0};//i点到j点最chang路径跳数（长度）
int longest_vu[maxnum][maxnum];//i-->j,longest path number
int MNnum;//当前mb数目
int MB[mm];//存储当前找到的mb
int vt,ut;//起点，终点
int node_shortest[maxnum]={-1}; //记录该路径通过哪个节点处理，默认为-1
int path_shortest[maxnum]={-1};//记录两点间通过哪条最短路径完成处理，默认为-1
double degree_node[maxnum]={0};//记录每个节点度数
int node[mm];//与correlation相对应节点编号
double correlation[maxnum]={0};//a*fre+b*degree
double frequency_node[maxnum]={0};//节点通过频率，根据历史流量分布统计
int hflow[FLOWNUM][5];//源host，目的host，包个数，包路径编号
int minfrequency,mindegree;
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
		//	cout<<endl;
		}
		searchPath(prev,v,prev[u][i],sta,len+1);
		//cout<<"->"<<u;
		whole_path[vt*n+ut][num_path[vt*n+ut]][js++]=u;
	}
}
//统计节点度数
void DegreeCalculate(int n, int pathlen[maxnum][maxnum]){
    for (int i = 0; i<n; i++){   //节点度
        for(int j = 0; j<n; j++){
            if(pathlen[i][j]<maxint){
                degree_node[i]++;
                degree_node[j]++;
            }
        }
    }
    int maxdegree=degree_node[0];//获得最大度数
    for(int i=1;i<n;i++){
        if(maxdegree<degree_node[i]){
            maxdegree=degree_node[i];
        }
    }
    mindegree=degree_node[0];//获得最xiao度数
    for(int i=1;i<n;i++){
        if(mindegree>degree_node[i]){
            mindegree=degree_node[i];
        }
    }
    //归一化
    for(int i=0;i<n;i++){
        degree_node[i]=(degree_node[i]-0.5*mindegree)*1.0/(maxdegree-mindegree);
    }
}
//统计历史流量情况下节点通过频率
void Frequency_Historyflow(int n){
    FILE* fp1=fopen("hflow.txt","r");
    for(int i=0;i<FLOWNUM;i++){
        for(int j=0;j<4;j++){
            fscanf(fp1,"%d",&hflow[i][j]);
        }
        fscanf(fp1,"\n");
    }
    fclose(fp1);
    int k;
    for(int i=0;i<FLOWNUM;i++){
        hflow_3[i][0]=hflow[i][0];
        hflow_3[i][1]=hflow[i][1];
        hflow_3[i][2]=hflow[i][2];
        k=hflow[i][4];
        int i1=hflow[i][0],j1=hflow[i][1];
        int nums=num_path[i1*n+j1];
		int numl=len_longest[i1*n+j1]+1;  //跳数，结点数目还需+1
		if(k<=nums) k=k;
		else k=0;
        for(int k1=0;k1<numl;k1++){
                //cout<<whole_path[i1*n+j1][k][k1]<<" ";
            if(whole_path[i1*n+j1][k][k1]!=0){
                frequency_node[whole_path[i1*n+j1][k][k1]]++;
            }
        }
    }
    int maxfrequency=frequency_node[0];//获得最大频率
    for(int i=1;i<n;i++){
        if(maxfrequency<frequency_node[i]){
            maxfrequency=frequency_node[i];
        }
    }
    minfrequency=frequency_node[0];//获得最xiao频率
    for(int i=1;i<n;i++){
        if(minfrequency>frequency_node[i]){
            minfrequency=frequency_node[i];
        }
    }
    //归一化
    for(int i=0;i<n;i++){
        frequency_node[i]=(frequency_node[i]-0.5*minfrequency)*1.0/(maxfrequency-minfrequency);
    }
}
void correlationlist(int a, int b, int n){
    for(int i=0;i<n;i++){
        correlation[i]=a*frequency_node[i]+b*degree_node[i];
    }
}
void Bubble_Sort(){
    for(int i=0;i<n;i++){
        node[i]=i;
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; i + j < n - 1; j++){
            if(correlation[j] > correlation[j + 1]){
                double temp = correlation[j];
                correlation[j] = correlation[j + 1];
                correlation[j + 1] = temp;
                int t=node[j];
                node[j]=node[j+1];
                node[j+1]=t;
            }
        }
    }
    double co[n];int no[n];
    for(int i=0; i<n;i++){
        co[n-i-1]=correlation[i];
        no[n-i-1]=node[i];
    }
    for(int i=0; i<n;i++){
        correlation[i]=co[i];
        node[i]=no[i];
    }
}
//读文件，获取两点间最长路径长度
void longpath(int n){
    FILE* fp2=fopen("lengtestpath.txt","r");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            fscanf(fp2,"%d ",&longest_vu[i][j]);
            if(longest_vu[i][j]==0){
                longest_vu[i][j]=shortest_pathlength[i][j];
            }
        }
        fscanf(fp2,"\n");
    }
    fclose(fp2);
//    for(int i=0;i<n;i++){
//        for(int j=0;j<n;j++){
//            printf("%d ",longest_vu[i][j]);
//        }
//        printf("\n");
//    }
}
//目标函数  longest_vu[i][j]*frequency_node[j]
//两种方法
//1，一次选MB_input个  2.每次选择一个，非邻接点继续选择
void Bubble_FlowSize(){
    for(int i = 0; i < FLOWNUM; i++){
        for(int j = 0; i + j < FLOWNUM - 1; j++){
            if(hflow_3[j][2] > hflow_3[j + 1][2]){
                int temp=hflow_3[j][2];
                hflow_3[j][2] = hflow_3[j+1][2];
                hflow_3[j+1][2] = temp;
                int temp1=hflow_3[j][1];
                hflow_3[j][1] = hflow_3[j+1][1];
                hflow_3[j+1][1] = temp1;
                int temp2=hflow_3[j][0];
                hflow_3[j][0] = hflow_3[j+1][0];
                hflow_3[j+1][0] = temp2;
            }
        }
    }
    int hf[n][4];
    for(int i=0; i<n;i++){
        hf[n-i-1][2]=hflow_3[i][2];
        hf[n-i-1][1]=hflow_3[i][1];
        hf[n-i-1][0]=hflow_3[i][0];
    }
    for(int i=0; i<n;i++){
        hflow_3[i][2]=hf[i][2];
        hflow_3[i][1]=hf[i][1];
        hflow_3[i][0]=hf[i][0];
    }
}

int maxlen_calculate(int MB_input, int MB[maxnum]){ //假定MB_input个MB已经选好,计算最小的flow size×min length
    int len,sum=0;
    int ki[maxnum]={0};
    //在吞吐量限制下选择MB，大流优先（最近最先）
    //int MBcapacity[maxnum]=300;//MB吞吐量限制    hflow[i][2], flow packets number
    Bubble_FlowSize();//按照包数目降序，包含源和目的
    for(int k=0;k<FLOWNUM;k++){
      //  len=shortest_pathlength[k][MB[0]];
        len = longest_vu[hflow_3[k][0]][MB[0]];  //获得src 到  最小的最远距离
        for(int i=0 ; i < MB_input ; i++){
            //最大距离, 吞吐量
            if(longest_vu[hflow_3[k][0]][MB[i]] < len){
                if( MBcapacity[i] > hflow_3[k][2]){
                    ki[k]=i;
                    len = longest_vu[hflow_3[k][0]][MB[i]];
                }
            }
        }
        MBcapacity[ki[k]] -= hflow_3[k][2];  //减去吞吐量消耗
        sum+=len*hflow_3[k][2];
    }
    return sum;
}
void Decision_1(int n, int MB_input){
    int flag[maxnum]={0};//可选择
    int flagg[maxnum]={0};//是否已被选择，被选择为1
    Bubble_Sort();
    for(int i=0;i<n;i++){
     //   cout<<correlation[i]<<endl;
    }
    //choose MB
    MB[0]=node[0];
    MNnum=1;
    flagg[node[0]]=1;//the first MB
    for(int j=1; j<n; j++){    //the second MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n", node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    } //the third MB
    for(int j=1; j<n; j++){    //the third MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    for(int j=1; j<n; j++){    //the forth MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    for(int j=1; j<n; j++){    //the 5 MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    for(int j=1; j<n; j++){    //the 6 MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    for(int j=1; j<n; j++){    //the 7 MB
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d\n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    //8
    for(int j=1; j<n; j++){
        if(flagg[j]==1){
            for(int jk=0;jk<n;jk++){
                if(pathlen[j][jk]<maxint){
                    flag[jk]=1;
                }
            }
        }
    }
    for(int j=1; j<n; j++){
        if(flag[node[j]]==0&&flagg[node[j]]==0){  //unconnected
            if(MNnum+1<=MB_input){
                MB[MNnum++] = node[j];
                printf("%d \n",node[j]);
                flagg[node[j]]=1;
                break;
            }
        }
    }
    printf("%d\n",node[0]);
    int sum=maxlen_calculate(MB_input,MB);
    printf("%d %d\n",MNnum,sum);
}

void Decision_2(int n, int MB_input){
    //choose MB
    int arr[n];
    for(int i=0;i<n;i++){
        arr[i]=i;
    }

    FILE* fp2=fopen("jiecheng.txt","r");
    for(int i=0;i<999999999;i++){
        for(int j=0;j<8;j++){
            fscanf(fp2,"%d ",&MB[j]);
        }
        fscanf(fp2,"%d ",&rest[i]);
        MB_result[i]=maxlen_calculate(MB_input, MB);
        fscanf(fp2,"\n");
    }
    fclose(fp2);
    int result[MB_input];
    int m=MB_result[0];
    int res=0;//记录第几条选择获得的开销最小
    for(int i=1;i<s;i++){  //获得最小的开销
        if(MB_result[i] < MB_result[0]){
            res = i;  //获得i，由jiecheng.cpp计算组合
        }
    }
    cout<<res<<" "<<MB_result[res]<<endl;
    FILE*fp=fopen("Result.txt","w");
    fprintf(fp,"%d",res);
    fprintf(fp,"%d",MB_result[res]);
    fclose(fp);
    cout<<res<<" "<<MB_result[res]<<endl;
}

void input(){
    for(int i=0;i<maxnum;i++){
		node_shortest[i]=-1;
		path_shortest[i]=-1;
	}
    vector<int> prev[maxnum];     // 记录当前点的前一个结点
	//cin>>n;	// 输入结点数
	//cin>>line;	// 输入路径数
	n=41;line=116;
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
 //   			cout << i<<"点到"<<j<<"的最短路径长度: " << dist[j] << endl;
    			shortest_pathlength[i][j]=dist[j];
 	//			cout << i<<"点到"<<j<<"的路径为: "<<endl;
				int sta[maxnum];
				searchPath(prev,i,j,sta,0);
    		}
    //		cout<<endl;
/*
			int nums=num_path[i*n+j];
			int numl=len_longest[i*n+j]+1;  //跳数，结点数目还需+1
			for(int k=0;k<nums;k++){
				for(int k1=0;k1<numl;k1++){   //跳数len_longest[i*n+j]    //all径数目num_path[i*n+j]
					cout<<"$$"<<whole_path[i*n+j][k][k1]<<" ";    //i到j路线起点whole_path[i*n+j][0][0]
				}                                        //终点whole_path[i*n+j][0][len_longest[i*n+j]
			}
			cout<<endl;
*/
    	}
    }
}
int main() {
    int a,b;//parameter,a*fre+b*degree
    input();
    //计算节点度
//    DegreeCalculate(n, pathlen);
  //  Frequency_Historyflow(n);
    //correlationlist(10,1, n);
    longpath(n);
    cout<<1<<endl;
    //Decision_1(n, 8);
    Decision_2(n,8);
    cout<<33<<endl;
}

/*
9 26

*/
