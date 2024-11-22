#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CampusNavigation.h"
#include "Graph.h"
#include "qstringlistmodel.h"
#include "PlaceItem.h"

class CampusNavigation : public QMainWindow
{
    Q_OBJECT

public:
    CampusNavigation(QWidget *parent = nullptr);
    ~CampusNavigation();

public slots:
    void on_addPlaceButton_clicked();
    void on_shortestPathButton_clicked(); // �������·���İ�ť

private:
    Ui::CampusNavigationClass ui;

    Graph campusMap;

    QStringListModel* placesModel; // ���� ListView ��ģ��
    QGraphicsScene* scene;     
    QGraphicsView* view;       // ������ʾ��������ͼ

    void drawCampusMap();
    void drawPath(const std::vector<Place>& path);

    void calculateShortestPath(int startId, int endId); // ���㲢��ʾ·��

    int startPlaceId = -1; // ��ʼ���� ID
    int endPlaceId = -1; // �յ㳡�� ID

    bool selectingStart = false; // �Ƿ�����ѡ����ʼ�ص�
    bool selectingEnd = false;   // �Ƿ�����ѡ���յ�
};
