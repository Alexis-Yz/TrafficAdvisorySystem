#pragma once

#include <QObject>
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_set>

const int MAX = 100;  // 假设城市数量不超过100
const int INF = std::numeric_limits<int>::max();

struct bestPath
{
	int src;
	int dest;
	std::vector<int> path;
};
class Graph  : public QObject
{
	Q_OBJECT

public:
	Graph(int V, QObject *parent);
	~Graph();

	void addCityEdge(int u, int v, int w);
	void addLengthEdge(int u, int v, int w);
	void optimizeLength();
	void dijkstra(int src, std::vector<int>& dist, int adjMatrix[MAX][MAX]);
	void saveDjPath(int src, int dest, std::vector<int>& parent);
	void saveFoPath(int src, int dest, std::vector<std::vector<int>>& next);
	void floyd(int src, std::vector<std::vector<int>>& dist, int adjMatrix[MAX][MAX]);
	QString printEdge(int flag);
	int getVValue();
	std::vector<bestPath> getBestPath();
	std::vector<int> getSingleLength(int src, int flag);
	std::vector<std::vector<int>> getMulLength(int src, int flag);

	void findAllPaths(int start, int end, int maxLength);
	// DFS
	void findAllPathsDFS(int current, int end, std::unordered_set<int>& visited, std::vector<int>& path, int maxLength, int currentLength);

	// K短路径算法
	std::vector<std::pair<int, std::vector<int>>> kShortestPaths(int start, int end, int k, int numNodes);


signals:
	void pathAll(QStringList, int);

private:
	int V;  // 顶点的数量
	int adjMatrixCity[MAX][MAX];  // 城市邻接矩阵
	int adjMatrixLength[MAX][MAX];	// 带距离的城市邻接矩阵
	std::vector<bestPath> myBestPath;	// 保存最短路径
};
