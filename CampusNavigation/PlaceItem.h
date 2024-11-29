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
    using PathSelectionCallback = std::function<void(int, int)>; // ����ص�����

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

    Place placeInfo; // �洢������Ϣ
    PathSelectionCallback pathCallback; // �洢·��ѡ��ص�

    static ItemMode currentMode; // ��ǰģʽ
    static int startPlaceId;      // ��ʼ�� ID
    static int endPlaceId;        // �յ� ID
    static bool selectingStart;    // �Ƿ�����ѡ����ʼ��
    static bool selectingEnd;      // �Ƿ�����ѡ���յ�

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (isSelected()) {
            if (currentMode == PathSelection) {
                // ·��ѡ���߼�
                if (selectingStart && startPlaceId == -1) {
                    startPlaceId = placeInfo.id; // ��¼��ʼ�� ID
                    QMessageBox::information(nullptr, QStringLiteral("ѡ����ʼ����"),
                        QStringLiteral("��ʼ���� ID: %1").arg(startPlaceId));
                    selectingStart = false;
                    selectingEnd = true; // �л���ѡ���յ�
                }
                else if (selectingEnd && endPlaceId == -1) {
                    endPlaceId = placeInfo.id; // ��¼�յ� ID
                    QMessageBox::information(nullptr, QStringLiteral("ѡ���յ㳡��"),
                        QStringLiteral("�յ㳡�� ID: %1").arg(endPlaceId));
                    // ����·������ص�
                    if (pathCallback) {
                        pathCallback(startPlaceId, endPlaceId);
                    }
                    resetSelection();
                }
            }
            else if (currentMode == ViewProperties) {
                // �鿴�����߼�
                qDebug() << "Clicked on place:" << placeInfo.name << "ID:" << placeInfo.id;
                QMessageBox::information(nullptr, QStringLiteral("��������"),
                    QStringLiteral("����: %1\n���: %2\n����: %3\n����: (%4, %5)")
                    .arg(placeInfo.name)
                    .arg(placeInfo.id)
                    .arg(placeInfo.type)
                    .arg(placeInfo.coordinates.first)
                    .arg(placeInfo.coordinates.second));
            }
        }

        // ������׼ѡ�����
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
