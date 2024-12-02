#ifndef PLACEITEM_H
#define PLACEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QMessageBox>
#include <QDebug>
#include "Place.h"
#include "Path.h"
#include <functional>

class PlaceItem : public QGraphicsEllipseItem {
public:
    using PathSelectionCallback = std::function<void(int, int)>; // 定义回调类型

    enum ItemMode {
        PathSelection,
        ViewProperties
    };

    PlaceItem(const Place& place,  PathSelectionCallback pathCallback, QGraphicsItem* parent = nullptr)
        : QGraphicsEllipseItem(parent), placeInfo(place), pathCallback(pathCallback)  {
        setRect(place.coordinates.first, place.coordinates.second, 10, 10);
        setBrush(QBrush(Qt::blue));
        setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);
    }

    Place placeInfo; // 存储场所信息
    PathSelectionCallback pathCallback; // 存储路径选择回调

    static ItemMode currentMode; // 当前模式
    static int startPlaceId;      // 起始点 ID
    static int endPlaceId;        // 终点 ID
    static bool selectingStart;    // 是否正在选择起始点
    static bool selectingEnd;      // 是否正在选择终点

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (isSelected()) {
            if (currentMode == PathSelection) {
                // 路径选择逻辑
                if (selectingStart && startPlaceId == -1) {
                    startPlaceId = placeInfo.id; // 记录起始点 ID
                    QMessageBox::information(nullptr, QStringLiteral("选择起始场所"),
                        QStringLiteral("起始场所 ID: %1").arg(startPlaceId));
                    selectingStart = false;
                    selectingEnd = true; // 切换到选择终点
                }
                else if (selectingEnd && endPlaceId == -1) {
                    endPlaceId = placeInfo.id; // 记录终点 ID
                    QMessageBox::information(nullptr, QStringLiteral("选择终点场所"),
                        QStringLiteral("终点场所 ID: %1").arg(endPlaceId));
                    // 调用路径计算回调
                    if (pathCallback) {
                        pathCallback(startPlaceId, endPlaceId);
                    }
                    resetSelection();
                }
            }
            else if (currentMode == ViewProperties) {
                // 查看属性逻辑
                qDebug() << "Clicked on place:" << placeInfo.name << "ID:" << placeInfo.id;
                QMessageBox::information(nullptr, QStringLiteral("场所属性"),
                    QStringLiteral("名称: %1\n编号: %2\n类型: %3\n坐标: (%4, %5)")
                    .arg(placeInfo.name)
                    .arg(placeInfo.id)
                    .arg(placeInfo.type)
                    .arg(placeInfo.coordinates.first)
                    .arg(placeInfo.coordinates.second));
            }
        }

        // 继续标准选择操作
        QGraphicsEllipseItem::mousePressEvent(event);
    }

    void resetSelection() {
        startPlaceId = -1;
        endPlaceId = -1;
        selectingStart = false;
        selectingEnd = false;
    }
};

#endif // PLACEITEM_H
