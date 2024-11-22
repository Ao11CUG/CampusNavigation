#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include "Place.h"

class Graph {
public:
    void addPlace(const Place& place); // ��ӳ���
    void addPath(int fromId, int toId, double distance); // ���·��
    std::unordered_map<int, Place> getPlaces(); // ��ȡ���г���
    std::vector<Place> findShortestPath(int startId, int endId); // �������·��

private:
    std::unordered_map<int, Place> places; // �洢����
    std::unordered_map<int, std::unordered_map<int, double>> adjacencyList; // �洢����ͼ���ڽӱ�
};

#endif // GRAPH_H
