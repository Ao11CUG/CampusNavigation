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
    void addPlace(const Place& place); // ��ӳ���
    void addPath(const Path& path); // ���·��
    std::unordered_map<int, Place> getPlaces(); // ��ȡ���г���
    std::unordered_map<int, Path> getPaths(); // ��ȡ���г���
    Path findShortestPath(int startId, int endId); // �������·��

private:
    std::unordered_map<int, Place> places; // �洢����
    std::unordered_map<int, Path> paths; // �洢����
    std::unordered_map<int, std::unordered_map<int, double>> adjacencyList; // �洢����ͼ���ڽӱ�
};

#endif // GRAPH_H
