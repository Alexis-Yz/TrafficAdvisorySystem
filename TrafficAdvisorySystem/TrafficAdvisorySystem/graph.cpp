#include "graph.h"

Graph::Graph(int V, QObject *parent)
	: V(V),
	QObject(parent)
{
	for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
        {
            // 初始化城市邻接矩阵
            adjMatrixCity[i][j] = (i == j) ? 1 : INF;
            // 初始化城市距离邻接矩阵
            adjMatrixLength[i][j] = (i == j) ? 0 : INF;
        }
			
}

Graph::~Graph()
{}

// 无向图添加边
void Graph::addCityEdge(int u, int v, int w)
{
	adjMatrixCity[u][v] = w;
	adjMatrixCity[v][u] = w;
}

void Graph::addLengthEdge(int u, int v, int w)
{
    adjMatrixLength[u][v] = w;
    adjMatrixLength[v][u] = w;
}

// 优化城市距离邻接矩阵
void Graph::optimizeLength()
{
    for(int i=0;i<V;i++)
        for (int j = 0; j < V; j++)
        {
            if (adjMatrixCity[i][j] == 0)
                adjMatrixLength[i][j] = INF;
        }
}


// 迪杰斯特拉算法
// 参数：源节点，源节点到其他节点的最短路径长度
void Graph::dijkstra(int src, std::vector<int>& dist, int adjMatrix[MAX][MAX])
{
    // 使用优先队列来选择下一个要处理的节点
    // pair<int, int>表示距离和节点的组合，greater确保队列按距离递增的顺序排列（小的在队头）
    // 默认先比较第一个元素，第一个相等则比较第二个
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
    // 用于标记节点是否已经被访问
    std::vector<bool> visited(V, false);

    // 初始化存储最短路径的向量为无穷大(INF)
    dist = std::vector<int>(V, INF);
    // 将源节点及其距离(0)放入优先队列
    pq.push(std::make_pair(0, src));
    // 将源节点到自身的距离设置为0
    dist[src] = 0;
    // 用于存储最短路径的父节点
    std::vector<int> parent(V, -1);
    // 进入主循环，直到优先队列为空
    while (!pq.empty()) 
    {
        // 从优先队列中取出当前距离最小的节点
        int u = pq.top().second;
        // 将该节点从优先队列中移除
        pq.pop();
        // 标记当前节点为已访问
        visited[u] = true;

        // 遍历与当前节点相邻的所有节点
        for (int v = 0; v < V; ++v)
        {
            // 检查如果节点未被访问、存在连接边、从源节点到当前节点的距离不是无穷大，并且通过当前节点的路径距离更短
            if (!visited[v] && adjMatrix[u][v]!=INF && adjMatrix[u][v] && dist[u] + adjMatrix[u][v] < dist[v])
            {
                // 更新从源节点到节点v的最短路径
                dist[v] = dist[u] + adjMatrix[u][v];
                // 记录最短路径的父节点
                parent[v] = u;
                // 将更新后的节点v及其新距离放入优先队列
                pq.push(std::make_pair(dist[v], v));
            }
        }
    }
    // 初始化保存最短路径的vector
    myBestPath.clear();
    myBestPath.shrink_to_fit();
    // 保存从源节点到其他节点的最短路径
    for (int i = 0; i < V; ++i)
    {
        if (i != src) 
        {
            saveDjPath(src, i, parent);
        }
    }
}

// 保存Dj最短路径
void Graph::saveDjPath(int src, int dest, std::vector<int>& parent)
{
    bestPath tempPath;
    tempPath.src = src;
    tempPath.dest = dest;
    std::stack<int> path;
    int current = dest;
    while (current != src) 
    {
        path.push(current);
        current = parent[current];
    }
    path.push(src);
    while (!path.empty()) 
    {
        tempPath.path.push_back(path.top());
        path.pop();
    }
    myBestPath.push_back(tempPath);
}

// 保存佛洛依德最短路径
void Graph::saveFoPath(int src, int dest, std::vector<std::vector<int>>& next)
{
    bestPath tempPath;
    tempPath.src = src;
    tempPath.dest = dest;
    std::vector<int> path;
    int current = src;
    while (current != dest) {
        path.push_back(current);
        current = next[current][dest];
    }
    path.push_back(dest);
    for (auto const& p : path)
    {
        tempPath.path.push_back(p);
    }
    myBestPath.push_back(tempPath);
}

// 佛洛依德算法
// 参数：所有节点之间的最短路径
void Graph::floyd(int src, std::vector<std::vector<int>>& dist, int adjMatrix[MAX][MAX])
{
    // 初始化一个二维向量 dist，用于存储所有节点之间的最短路径长度，初始值为无穷大(INF)
    dist = std::vector<std::vector<int>>(V, std::vector<int>(V, INF));
    // 将对角线上的元素设置为0，表示节点到自身的距离为0
    for (int i = 0; i < V; i++) {
        dist[i][i] = 0;
    }

    // 用于存储路径中下一个节点的信息
    std::vector<std::vector<int>> next(V, std::vector<int>(V, -1));
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
        {
            if (i != j && adjMatrix[i][j] != INF) {
                // 复制邻接矩阵的初始值到 dist 中
                dist[i][j] = adjMatrix[i][j];
                next[i][j] = j; // 如果有连接边，设置下一个节点为目标节点 j
            }
        }
           

    // 在三重循环中进行动态规划，遍历所有节点 k，更新从节点 i 到节点 j 的最短路径
    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                // 如果通过节点 k 能够获得更短的路径
                if (dist[i][k] + dist[k][j] < dist[i][j] && dist[i][k]!=INF && dist[k][j]!=INF)
                {
                    // 更新最短路径的值
                    dist[i][j] = dist[i][k] + dist[k][j];
                    // 更新路径中下一个节点的信息
                    next[i][j] = next[i][k]; 
                }
            }
        }
    }
    // 初始化保存最短路径的vector
    myBestPath.clear();
    myBestPath.shrink_to_fit();
    // 保存从源节点到其他节点的最短路径
    for (int i = 0; i < V; ++i)
    {
        if (i != src)
        {
            saveFoPath(src, i, next);
        }
    }
}

QString Graph::printEdge(int flag)
{
    QString edge="  | ";
    int i, j;
    for (i = 0; i < V; i++)
    {
        edge += QString::number(i);
        edge += " | ";
    }
    edge += "\n";
    for (i = 0; i < V; i++)
    {
        edge += QString::number(i);
        edge += " | ";
        for (j = 0; j < V; j++)
        {
            if (flag == 1)
            {
                edge += QString::number(adjMatrixCity[i][j]);
            }
            else if (flag == 2)
            {
                edge += QString::number(adjMatrixLength[i][j]);
            }
            edge += " | ";
        }
        edge += '\n';
    }
    return edge;
}

int Graph::getVValue()
{
    return V;
}

std::vector<bestPath> Graph::getBestPath()
{
    return myBestPath;
}

// 迪杰斯特拉计算单源最短路径
std::vector<int> Graph::getSingleLength(int src, int flag)
{
    std::vector<int> myDijkstra;
    if (flag == 1) dijkstra(src, myDijkstra, adjMatrixCity);
    if (flag == 2) dijkstra(src, myDijkstra, adjMatrixLength);
    return myDijkstra;
}

// 弗洛伊德计算单源最短路径
std::vector<std::vector<int>> Graph::getMulLength(int src, int flag)
{
    std::vector<std::vector<int>> myFloyd;
    if (flag == 1) floyd(src, myFloyd, adjMatrixCity);
    if (flag == 2) floyd(src, myFloyd, adjMatrixLength);
    return myFloyd;
}

void Graph::findAllPaths(int start, int end, int maxLength)
{
    std::vector<int> path;
    // 创建一个无序集合
    std::unordered_set<int> visited;
    int currentLength = 0;
    findAllPathsDFS(start, end, visited, path, maxLength, currentLength);
}

void Graph::findAllPathsDFS(int current, int end, std::unordered_set<int>& visited, std::vector<int>& path, int maxLength, int currentLength)
{
    // 向无序集合中插入当前节点
    visited.insert(current);
    // 将当前节点加入路径
    path.push_back(current);

    if (path.size() <= maxLength) 
    {
        if (current == end)
        {
            QStringList ll;
            // 生成路径
            for (int node : path) 
            {
                ll<< QString::number(node);
            }
            emit pathAll(ll, currentLength);
        }
        else
        {
            for (int neighbor = 0; neighbor < V; ++neighbor)
            {
                // 邻边存在直达路径，并且相邻的节点还未被访问
                if (adjMatrixLength[current][neighbor] != INF && neighbor!=current && visited.find(neighbor) == visited.end()) 
                {
                    int edgeWeight = adjMatrixLength[current][neighbor];
                    currentLength += edgeWeight;
                    findAllPathsDFS(neighbor, end, visited, path, maxLength,currentLength);
                    currentLength -= edgeWeight; // 回溯时减去当前边的长度
                }
            }
        }
    }
    // 从集合中删除current
    visited.erase(current);
    // 删除最后一个元素
    path.pop_back();
}

std::vector<std::pair<int, std::vector<int>>> Graph::kShortestPaths(int start, int end, int k, int numNodes)
{
    //std::vector<std::vector<int>> result;
    std::vector<std::pair<int, std::vector<int>>> result;

    // 优先队列，用于选择当前最短路径
    std::priority_queue<std::pair<int, std::vector<int>>, std::vector<std::pair<int, std::vector<int>>>, std::greater<std::pair<int, std::vector<int>>>> pq;

    // 将起始节点加入队列
    pq.push({ 0, {start} });

    while (!pq.empty()) 
    {
        // 取出当前最短路径
        auto current = pq.top();
        pq.pop();
        int currentCost = current.first;
        std::vector<int> currentPath = current.second;
        // 取出路径的最后一个节点
        int currentNode = currentPath.back();
        // 如果到达目标节点，将路径加入结果集
        if (currentNode == end) 
        {
            std::pair<int, std::vector<int>> myPair = std::make_pair(currentCost, currentPath);
            result.push_back(myPair);
            if (result.size() == k)
            {
                break;  // 找到前K短路径，结束循环
            }
            continue;
        }
        // 遍历邻居节点
        for (int neighbor = 0; neighbor < numNodes; ++neighbor) 
        {
            if (adjMatrixLength[currentNode][neighbor] != INF && currentNode != neighbor && std::find(currentPath.begin(), currentPath.end(), neighbor) == currentPath.end()) 
            {
                // 创建新路径
                std::vector<int> newPath = currentPath;
                newPath.push_back(neighbor);

                // 计算新路径的代价
                int newCost = currentCost + adjMatrixLength[currentNode][neighbor];

                // 将新路径加入优先队列
                pq.push({ newCost, newPath });
            }
        }
    }
    return result;
}
