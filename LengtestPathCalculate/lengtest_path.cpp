// A C++ program to find single source longest distances in a DAG
#include <iostream>
#include <list>
#include <stack>
#include <limits.h>
#include<stdio.h>
#define NINF INT_MIN
using namespace std;
FILE *fp;
//图通过邻接表来描述。邻接表中的每个顶点包含所连接的顶点的数据，以及边的权值。
class AdjListNode
{
    int v;
    int weight;
public:
    AdjListNode(int _v, int _w)  { v = _v;  weight = _w;}
    int getV()       {  return v;  }
    int getWeight()  {  return weight; }
};

// Class to represent a graph using adjacency list representation
class Graph
{
    int V;    // No. of vertices’

    // Pointer to an array containing adjacency lists
    list<AdjListNode> *adj;

    // A function used by longestPath
    void topologicalSortUtil(int v, bool visited[], stack<int> &Stack);
public:
    Graph(int V);   // Constructor

    // function to add an edge to graph
    void addEdge(int u, int v, int weight);

    // Finds longest distances from given source vertex
    void longestPath(int s);
};

Graph::Graph(int V) // Constructor
{
    this->V = V;
    adj = new list<AdjListNode>[V];
}

void Graph::addEdge(int u, int v, int weight)
{
    AdjListNode node(v, weight);
    adj[u].push_back(node); // Add v to u’s list
}

// 通过递归求出拓扑序列. 详细描述，可参考下面的链接。
// http://www.geeksforgeeks.org/topological-sorting/
void Graph::topologicalSortUtil(int v, bool visited[], stack<int> &Stack)
{
    // 标记当前顶点为已访问
    visited[v] = true;

    // 对所有邻接点执行递归调用
    list<AdjListNode>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
    {
        AdjListNode node = *i;
        if (!visited[node.getV()])
            topologicalSortUtil(node.getV(), visited, Stack);
    }

    // 当某个点没有邻接点时，递归结束，将该点存入栈中。
    Stack.push(v);
}
// 根据传入的顶点，求出到到其它点的最长路径. longestPath使用了
// topologicalSortUtil() 方法获得顶点的拓扑序。
void Graph::longestPath(int s)
{
    stack<int> Stack;
    int dist[V];

    // 标记所有的顶点为未访问
    bool *visited = new bool[V];
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // 对每个顶点调用topologicalSortUtil，最终求出图的拓扑序列存入到Stack中。
    for (int i = 0; i < V; i++)
        if (visited[i] == false)
            topologicalSortUtil(i, visited, Stack);

    //初始化到所有顶点的距离为负无穷
    //到源点的距离为0
    for (int i = 0; i < V; i++)
        dist[i] = NINF;
    dist[s] = 0;

    // 处理拓扑序列中的点
    while (Stack.empty() == false)
    {
        //取出拓扑序列中的第一个点
        int u = Stack.top();
        Stack.pop();

        // 更新到所有邻接点的距离
        list<AdjListNode>::iterator i;
        if (dist[u] != NINF)
        {
          for (i = adj[u].begin(); i != adj[u].end(); ++i)
             if (dist[i->getV()] < dist[u] + i->getWeight())
                dist[i->getV()] = dist[u] + i->getWeight();
        }
    }

    // 打印最长路径
    for (int i = 0; i < V; i++){
        (dist[i] == NINF)? cout << "INF ": cout << dist[i] << " ";
        (dist[i] == NINF)? fprintf(fp,"%d ", 0): fprintf(fp,"%d ", dist[i]);
    }
    fprintf(fp,"\n");
    //将最长距离写入文件

}

// Driver program to test above functions
int main()
{
    Graph g(41);
    g.addEdge(	0	,	6	,	1	);

    fp=fopen("lengtestpath.txt","w");

    int s=0;
    cout << "Following are longest distances from source vertex " << s <<" \n";
    for(int s=0;s<50;s++){
        g.longestPath(s);
        cout<<endl;
    }
    fclose(fp);

    return 0;
}
