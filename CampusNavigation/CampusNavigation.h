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
    void on_shortestPathButton_clicked(); // 计算最短路径的按钮

private:
    Ui::CampusNavigationClass ui;

    Graph campusMap;

    QStringListModel* placesModel; // 用于 ListView 的模型
    QGraphicsScene* scene;     
    QGraphicsView* view;       // 用于显示场景的视图

    void drawCampusMap();
    void drawPath(const std::vector<Place>& path);

    void calculateShortestPath(int startId, int endId); // 计算并显示路径

    int startPlaceId = -1; // 起始场所 ID
    int endPlaceId = -1; // 终点场所 ID

    bool selectingStart = false; // 是否正在选择起始地点
    bool selectingEnd = false;   // 是否正在选择终点
};
