#include "CampusNavigation.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <qdialog.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qgraphicsitem.h"
#include "QPen"

CampusNavigation::CampusNavigation(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // ���� QGraphicsScene �� QGraphicsView
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);
    ui.scrollArea->setWidget(view);  // �� QGraphicsView ����Ϊ scrollArea ���ӿؼ�

    // ��ʼ�� ListView ��ģ��
    placesModel = new QStringListModel(this);
    ui.listView->setModel(placesModel);
}

CampusNavigation::~CampusNavigation()
{}

void CampusNavigation::on_addPlaceButton_clicked() {
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("��ӳ���"));

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    QLineEdit* idInput = new QLineEdit(&dialog);
    QLineEdit* typeInput = new QLineEdit(&dialog);
    QLabel* coordinatesLabel = new QLabel(QStringLiteral("���� (x, y):"), &dialog);
    QLineEdit* xInput = new QLineEdit(&dialog);
    QLineEdit* yInput = new QLineEdit(&dialog);

    layout->addWidget(new QLabel(QStringLiteral("���볡�����ƣ�")));
    layout->addWidget(nameInput);
    layout->addWidget(new QLabel(QStringLiteral("���볡����ţ�")));
    layout->addWidget(idInput);
    layout->addWidget(new QLabel(QStringLiteral("���볡�����")));
    layout->addWidget(typeInput);
    layout->addWidget(coordinatesLabel);
    layout->addWidget(new QLabel(QStringLiteral("X���꣺"), &dialog));
    layout->addWidget(xInput);
    layout->addWidget(new QLabel(QStringLiteral("Y���꣺"), &dialog));
    layout->addWidget(yInput);

    QPushButton* addButton = new QPushButton(QStringLiteral("���"), &dialog);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, [&dialog, this, nameInput, idInput, typeInput, xInput, yInput]() {
        bool idOk;
        int id = idInput->text().toInt(&idOk);
        bool xOk, yOk;
        double x = xInput->text().toDouble(&xOk);
        double y = yInput->text().toDouble(&yOk);

        if (idOk && xOk && yOk && !nameInput->text().isEmpty() && !typeInput->text().isEmpty()) {
            Place newPlace{ id, nameInput->text(), {x, y}, typeInput->text() }; // �����µĳ���

            campusMap.addPlace(newPlace); // ��ӵ�ͼ��
            drawCampusMap(); // ���Ƹ��º��У԰��ͼ

            // ���� ListView
            QStringList placesList;
            for (const auto& place : campusMap.getPlaces()) {
                placesList << place.second.name;
            }
            placesModel->setStringList(placesList); // �����б�ģ��

            dialog.accept(); // �رնԻ���
        }
        else {
            QMessageBox::warning(&dialog, QStringLiteral("����"), QStringLiteral("����д������Ϣ��"));
        }
        });

    dialog.exec();
}

void CampusNavigation::on_shortestPathButton_clicked() {
    PlaceItem::startPlaceId = -1; // Reset starting point ID
    PlaceItem::endPlaceId = -1;   // Reset ending point ID

    // ��ʾ�û���ʼѡ����ʼ���յ�
    QMessageBox::information(this, QStringLiteral("�������·��"), QStringLiteral("������ʼ�������յ㳡����"));

    // ����״̬��׼��·��ѡ��ģʽ
    PlaceItem::currentMode = PlaceItem::PathSelection; // ��Ϊ·��ѡ��ģʽ

    // ����״̬��־������ѡ����ʼ���յ�
    PlaceItem::selectingStart = true; // �û�������ѡ����ʼ��
    PlaceItem::selectingEnd = false;   // ��ʼ��Ϊ��δѡ���յ�
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
            PlaceItem* placeItem = new PlaceItem(place);

            // �����ź����
            //connect(placeItem, &PlaceItem::placesSelected, this, &CampusNavigation::calculateShortestPath);

            scene->addItem(placeItem);
        }
    }
}

void CampusNavigation::calculateShortestPath(int startId, int endId) {
    // ����Ƿ�ѡ������Ч����ʼ���յ�
    if (startId == -1 || endId == -1) {
        QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��ȷ����ѡ������������"));
        return;
    }

    // ʹ��ͼ������·���㷨����·��
    auto path = campusMap.findShortestPath(startId, endId);

    // ��ʾ���
    if (path.empty()) {
        QMessageBox::information(this, QStringLiteral("·�����"), QStringLiteral("�޷��ҵ�����ʼ�ص㵽�յ��·����"));
    }
    else {
        QString pathResult = QStringLiteral("���·��: ");
        for (const auto& place : path) {
            pathResult += QString::number(place.id) + " -> ";
        }
        pathResult.chop(4); // �Ƴ����� " -> "
        QMessageBox::information(this, QStringLiteral("·�����"), pathResult);
    }

    // �ָ�ģʽΪ�鿴����
    PlaceItem::currentMode = PlaceItem::ViewProperties; // �ָ�Ϊ�鿴����ģʽ

    // ����·��
    drawPath(path);
}

void CampusNavigation::drawPath(const std::vector<Place>& path) {
    // ���֮ǰ��·��
    for (auto item : scene->items()) {
        if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
            scene->removeItem(lineItem);
            delete lineItem; // ɾ��·��
        }
    }

    // ��·������Ϊ����
    for (size_t i = 0; i < path.size() - 1; ++i) {
        const Place& startPlace = path[i];
        const Place& endPlace = path[i + 1];

        // �����߶�
        QGraphicsLineItem* lineItem = new QGraphicsLineItem(startPlace.coordinates.first,
            startPlace.coordinates.second,
            endPlace.coordinates.first,
            endPlace.coordinates.second);
        lineItem->setPen(QPen(Qt::red, 2)); // ����������ɫ�Ϳ��
        scene->addItem(lineItem); // ��ӵ�������
    }
}