#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include "Place.h"
#include "Path.h"

class Graph {
public:
    void addPlace(const Place& place); // 添加场所
    void addPath(const Path& path); // 添加路径
    std::unordered_map<int, Place> getPlaces(); // 获取所有场所
    std::unordered_map<int, Path> getPaths(); // 获取所有场所
    Path findShortestPath(int startId, int endId); // 计算最短路径

private:
    std::unordered_map<int, Place> places; // 存储场所
    std::unordered_map<int, Path> paths; // 存储场所
    std::unordered_map<int, std::unordered_map<int, double>> adjacencyList; // 存储无向图的邻接表
};

#endif // GRAPH_H
