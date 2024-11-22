#include "Graph.h"
#include <iostream>

void Graph::addPlace(const Place& place) {
    places[place.id] = place; // ʹ�ñ����Ϊ���洢����
}

void Graph::addPath(int fromId, int toId, double distance) {
    adjacencyList[fromId][toId] = distance; // ���·��
    adjacencyList[toId][fromId] = distance; // ����ͼ����ӷ���·��
}

std::unordered_map<int, Place> Graph::getPlaces() {
    return places; // �������г���
}

std::vector<Place> Graph::findShortestPath(int startId, int endId) {
    // Dijkstra �㷨
    std::unordered_map<int, double> distances; // �洢����㵽����ľ���
    std::unordered_map<int, int> previous; // ��¼������·��
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> minHeap;

    // ��ʼ������
    for (const auto& place : places) {
        distances[place.first] = std::numeric_limits<double>::infinity(); // ��ʼ��Ϊ�����
    }
    distances[startId] = 0; // ��㵽�Լ��ľ���Ϊ 0
    minHeap.push({ 0, startId }); // �����������ȶ���

    while (!minHeap.empty()) {
        int currentId = minHeap.top().second;
        double currentDistance = minHeap.top().first;
        minHeap.pop();

        // ��ǰ����������
        if (currentId == endId) {
            break;
        }

        // �����ڽӽڵ�
        for (const auto& neighbor : adjacencyList[currentId]) {
            int neighborId = neighbor.first;
            double edgeWeight = neighbor.second;

            double newDistance = currentDistance + edgeWeight; // �����¾���
            if (newDistance < distances[neighborId]) {
                distances[neighborId] = newDistance; // ���¾���
                previous[neighborId] = currentId; // ����·��
                minHeap.push({ newDistance, neighborId }); // ��ӵ����ȶ���
            }
        }
    }

    // ����·��
    std::vector<Place> path;
    for (int at = endId; at != startId; at = previous[at]) {
        path.push_back(places[at]);
        if (previous.find(at) == previous.end()) {
            return {}; // ���û��·�������ؿ�
        }
    }
    path.push_back(places[startId]); // ������
    std::reverse(path.begin(), path.end()); // ��ת·��˳��

    return path; // �������·��
}
