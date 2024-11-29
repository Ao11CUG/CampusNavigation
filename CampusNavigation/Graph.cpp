#include "Graph.h"
#include <iostream>

void Graph::addPlace(const Place& place) {
    places[place.id] = place; // ʹ�ñ����Ϊ���洢����
}

void Graph::addPath(const Path& path) {
    adjacencyList[path.startPlace.id][path.endPlace.id] = path.distance; // ���·��
    adjacencyList[path.endPlace.id][path.startPlace.id] = path.distance; // ����ͼ����ӷ���·��
}

std::unordered_map<int, Place> Graph::getPlaces() {
    return places; // �������г���
}

std::unordered_map<int, Path> Graph::getPaths() {
    return paths; // �������г���
}

Path Graph::findShortestPath(int startId, int endId) {
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
                previous[neighborId] = currentId; // ���� PATH
                minHeap.push({ newDistance, neighborId }); // ��ӵ����ȶ���
            }
        }
    }

    // ����·��
    if (previous.find(endId) == previous.end()) {
        return Path(places[startId], places[endId], std::numeric_limits<double>::infinity()); // ������Ч·��
    }

    std::vector<Place> pathPlaces;
    for (int at = endId; at != startId; at = previous[at]) {
        pathPlaces.push_back(places[at]);
        if (previous.find(at) == previous.end()) {
            return Path(places[startId], places[endId], std::numeric_limits<double>::infinity()); // ���û��·����������Ч·��
        }
    }
    pathPlaces.push_back(places[startId]); // ������
    std::reverse(pathPlaces.begin(), pathPlaces.end()); // ��ת·��˳��

    // ���������� Path ����
    return Path(pathPlaces.front(), pathPlaces.back(), distances[endId]);
}

