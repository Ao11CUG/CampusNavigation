#include "CampusNavigation.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qgraphicsitem.h"
#include "QPen"

CampusNavigation::CampusNavigation(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // 创建 QGraphicsScene 和 QGraphicsView
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);
    ui.scrollArea->setWidget(view);  // 将 QGraphicsView 设置为 scrollArea 的子控件

    // 初始化 ListView 的模型
    placesModel = new QStringListModel(this);
    ui.listView->setModel(placesModel);
}

CampusNavigation::~CampusNavigation()
{}

void CampusNavigation::on_addPlaceButton_clicked() {
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("添加场所"));

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    QLineEdit* idInput = new QLineEdit(&dialog);
    QLineEdit* typeInput = new QLineEdit(&dialog);
    QLabel* coordinatesLabel = new QLabel(QStringLiteral("坐标 (x, y):"), &dialog);
    QLineEdit* xInput = new QLineEdit(&dialog);
    QLineEdit* yInput = new QLineEdit(&dialog);

    layout->addWidget(new QLabel(QStringLiteral("输入场所名称：")));
    layout->addWidget(nameInput);
    layout->addWidget(new QLabel(QStringLiteral("输入场所编号：")));
    layout->addWidget(idInput);
    layout->addWidget(new QLabel(QStringLiteral("输入场所类别：")));
    layout->addWidget(typeInput);
    layout->addWidget(coordinatesLabel);
    layout->addWidget(new QLabel(QStringLiteral("X坐标："), &dialog));
    layout->addWidget(xInput);
    layout->addWidget(new QLabel(QStringLiteral("Y坐标："), &dialog));
    layout->addWidget(yInput);

    QPushButton* addButton = new QPushButton(QStringLiteral("添加"), &dialog);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, [&dialog, this, nameInput, idInput, typeInput, xInput, yInput]() {
        bool idOk;
        int id = idInput->text().toInt(&idOk);
        bool xOk, yOk;
        double x = xInput->text().toDouble(&xOk);
        double y = yInput->text().toDouble(&yOk);

        if (idOk && xOk && yOk && !nameInput->text().isEmpty() && !typeInput->text().isEmpty()) {
            Place newPlace{ id, nameInput->text(), {x, y}, typeInput->text() }; // 创建新的场所

            campusMap.addPlace(newPlace); // 添加到图中
            drawCampusMap(); // 绘制更新后的校园地图

            // 更新 ListView
            QStringList placesList;
            for (const auto& place : campusMap.getPlaces()) {
                placesList << place.second.name;
            }
            placesModel->setStringList(placesList); // 更新列表模型

            dialog.accept(); // 关闭对话框
        }
        else {
            QMessageBox::warning(&dialog, QStringLiteral("错误"), QStringLiteral("请填写完整信息。"));
        }
        });

    dialog.exec();
}

void CampusNavigation::on_addPathButton_clicked() {
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("添加路径"));

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QComboBox* startBox = new QComboBox(&dialog);
    QComboBox* endBox = new QComboBox(&dialog);
    QLineEdit* distanceInput = new QLineEdit(&dialog);

    // 填充 combobox 的选项
    const auto& places = campusMap.getPlaces(); // 获取已有的场所
    for (const auto& pair : places) {
        const Place& place = pair.second; // 获取场所信息
        startBox->addItem(place.name, QVariant(place.id)); // 使用场所名称作为显示文本，用 ID 作为数据
        endBox->addItem(place.name, QVariant(place.id));   // 同样为终点 combobox 填充
    }

    layout->addWidget(new QLabel(QStringLiteral("选择起点：")));
    layout->addWidget(startBox);
    layout->addWidget(new QLabel(QStringLiteral("选择终点：")));
    layout->addWidget(endBox);
    layout->addWidget(new QLabel(QStringLiteral("距离权重："), &dialog));
    layout->addWidget(distanceInput);

    QPushButton* addButton = new QPushButton(QStringLiteral("添加"), &dialog);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, [&dialog, this, startBox, endBox, distanceInput]() {
        bool ok;
        double distance = distanceInput->text().toDouble(&ok); // 获取距离权重
        if (ok && distance > 0) {
            int startId = startBox->currentData().toInt(); // 获取选中起点的 ID
            int endId = endBox->currentData().toInt(); // 获取选中终点的 ID

            // 根据起点和终点创建路径
            Place startPlace = campusMap.getPlaces().at(startId);
            Place endPlace = campusMap.getPlaces().at(endId);
            Path newPath{ startPlace, endPlace, distance }; // 创建新的路径
            drawPath(newPath);

            campusMap.addPath(newPath); // 将路径添加到地图中
            drawCampusMap(); // 绘制更新后的校园地图

            dialog.accept(); // 关闭对话框
        }
        else {
            QMessageBox::warning(&dialog, QStringLiteral("错误"), QStringLiteral("请填写有效的距离。"));
        }
        });

    dialog.exec();
}

void CampusNavigation::on_shortestPathButton_clicked() {
    PlaceItem::startPlaceId = -1; // Reset starting point ID
    PlaceItem::endPlaceId = -1;   // Reset ending point ID

    // 提示用户开始选择起始和终点
    QMessageBox::information(this, QStringLiteral("计算最短路径"), QStringLiteral("请点击起始场所和终点场所。"));

    // 设置状态，准备路径选择模式
    PlaceItem::currentMode = PlaceItem::PathSelection; // 设为路径选择模式

    // 设置状态标志，正在选择起始和终点
    PlaceItem::selectingStart = true; // 用户将首先选择起始点
    PlaceItem::selectingEnd = false;   // 初始化为尚未选择终点
}

void CampusNavigation::drawCampusMap() {
    std::unordered_map<int, Place> places = campusMap.getPlaces();

    for (const auto& pair : places) {
        const Place& place = pair.second;

        bool exists = false;
        for (auto item : scene->items()) {
            if (auto placeItem = dynamic_cast<PlaceItem*>(item)) {
                if (placeItem->placeInfo.id == place.id) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            // 创建 PlaceItem，并传递回调函数
            PlaceItem* placeItem = new PlaceItem(place,
                [this](int startId, int endId) {
                    this->calculateShortestPath(startId, endId); // 调用计算最短路径
                });

            scene->addItem(placeItem);

            // 创建并设置 QGraphicsTextItem 来显示场所名称
            QGraphicsTextItem* textItem = new QGraphicsTextItem(place.name); // 使用场所名称
            textItem->setPos(place.coordinates.first + 15, place.coordinates.second); 
            scene->addItem(textItem);
        }
    }
}

void CampusNavigation::calculateShortestPath(int startId, int endId) {
    // 检查是否选择了有效的起始和终点
    if (startId == -1 || endId == -1) {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("请确保已选择两个场所。"));
        return;
    }

    // 使用图类的最短路径算法计算路径
    Path path = campusMap.findShortestPath(startId, endId);

    QString pathResult = QStringLiteral("最短路径: ");
    pathResult += QString(path.startPlace.name) + " -> " + QString(path.endPlace.name);

    highlightPath(path);
        
    QMessageBox::information(this, QStringLiteral("路径结果"), pathResult);

    // 恢复模式为查看属性
    PlaceItem::currentMode = PlaceItem::ViewProperties; // 恢复为查看属性模式
}

void CampusNavigation::drawPath(const Path& path) {
        // 创建线段
        QGraphicsLineItem* lineItem = new QGraphicsLineItem(path.startPlace.coordinates.first + 5,
            path.startPlace.coordinates.second + 5,
            path.endPlace.coordinates.first + 5,
            path.endPlace.coordinates.second + 5);
        lineItem->setPen(QPen(Qt::green, 2)); // 设置线条颜色和宽度
        scene->addItem(lineItem); // 添加到场景中
}

void CampusNavigation::highlightPath(const Path& path) {
    // 创建线段
    QGraphicsLineItem* lineItem = new QGraphicsLineItem(path.startPlace.coordinates.first + 5,
        path.startPlace.coordinates.second + 5,
        path.endPlace.coordinates.first + 5,
        path.endPlace.coordinates.second + 5);
    lineItem->setPen(QPen(Qt::red, 2)); // 设置线条颜色和宽度
    scene->addItem(lineItem); // 添加到场景中
}