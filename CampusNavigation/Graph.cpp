#include "Graph.h"
#include <iostream>

void Graph::addPlace(const Place& place) {
    places[place.id] = place; // 使用编号作为键存储场所
}

void Graph::addPath(int fromId, int toId, double distance) {
    adjacencyList[fromId][toId] = distance; // 添加路径
    adjacencyList[toId][fromId] = distance; // 无向图，添加反向路径
}

std::unordered_map<int, Place> Graph::getPlaces() {
    return places; // 返回所有场所
}

std::vector<Place> Graph::findShortestPath(int startId, int endId) {
    // Dijkstra 算法
    std::unordered_map<int, double> distances; // 存储从起点到各点的距离
    std::unordered_map<int, int> previous; // 记录经过的路径
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> minHeap;

    // 初始化距离
    for (const auto& place : places) {
        distances[place.first] = std::numeric_limits<double>::infinity(); // 初始化为无穷大
    }
    distances[startId] = 0; // 起点到自己的距离为 0
    minHeap.push({ 0, startId }); // 将起点加入优先队列

    while (!minHeap.empty()) {
        int currentId = minHeap.top().second;
        double currentDistance = minHeap.top().first;
        minHeap.pop();

        // 提前结束的条件
        if (currentId == endId) {
            break;
        }

        // 遍历邻接节点
        for (const auto& neighbor : adjacencyList[currentId]) {
            int neighborId = neighbor.first;
            double edgeWeight = neighbor.second;

            double newDistance = currentDistance + edgeWeight; // 计算新距离
            if (newDistance < distances[neighborId]) {
                distances[neighborId] = newDistance; // 更新距离
                previous[neighborId] = currentId; // 更新路径
                minHeap.push({ newDistance, neighborId }); // 添加到优先队列
            }
        }
    }

    // 回溯路径
    std::vector<Place> path;
    for (int at = endId; at != startId; at = previous[at]) {
        path.push_back(places[at]);
        if (previous.find(at) == previous.end()) {
            return {}; // 如果没有路径，返回空
        }
    }
    path.push_back(places[startId]); // 添加起点
    std::reverse(path.begin(), path.end()); // 反转路径顺序

    return path; // 返回最短路径
}
