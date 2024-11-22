#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include "Place.h"

class Graph {
public:
    void addPlace(const Place& place); // 添加场所
    void addPath(int fromId, int toId, double distance); // 添加路径
    std::unordered_map<int, Place> getPlaces(); // 获取所有场所
    std::vector<Place> findShortestPath(int startId, int endId); // 计算最短路径

private:
    std::unordered_map<int, Place> places; // 存储场所
    std::unordered_map<int, std::unordered_map<int, double>> adjacencyList; // 存储无向图的邻接表
};

#endif // GRAPH_H
