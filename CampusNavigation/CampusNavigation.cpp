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

void CampusNavigation::on_addPathButton_clicked() {
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("���·��"));

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QComboBox* startBox = new QComboBox(&dialog);
    QComboBox* endBox = new QComboBox(&dialog);
    QLineEdit* distanceInput = new QLineEdit(&dialog);

    // ��� combobox ��ѡ��
    const auto& places = campusMap.getPlaces(); // ��ȡ���еĳ���
    for (const auto& pair : places) {
        const Place& place = pair.second; // ��ȡ������Ϣ
        startBox->addItem(place.name, QVariant(place.id)); // ʹ�ó���������Ϊ��ʾ�ı����� ID ��Ϊ����
        endBox->addItem(place.name, QVariant(place.id));   // ͬ��Ϊ�յ� combobox ���
    }

    layout->addWidget(new QLabel(QStringLiteral("ѡ����㣺")));
    layout->addWidget(startBox);
    layout->addWidget(new QLabel(QStringLiteral("ѡ���յ㣺")));
    layout->addWidget(endBox);
    layout->addWidget(new QLabel(QStringLiteral("����Ȩ�أ�"), &dialog));
    layout->addWidget(distanceInput);

    QPushButton* addButton = new QPushButton(QStringLiteral("���"), &dialog);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, [&dialog, this, startBox, endBox, distanceInput]() {
        bool ok;
        double distance = distanceInput->text().toDouble(&ok); // ��ȡ����Ȩ��
        if (ok && distance > 0) {
            int startId = startBox->currentData().toInt(); // ��ȡѡ������ ID
            int endId = endBox->currentData().toInt(); // ��ȡѡ���յ�� ID

            // ���������յ㴴��·��
            Place startPlace = campusMap.getPlaces().at(startId);
            Place endPlace = campusMap.getPlaces().at(endId);
            Path newPath{ startPlace, endPlace, distance }; // �����µ�·��
            drawPath(newPath);

            campusMap.addPath(newPath); // ��·����ӵ���ͼ��
            drawCampusMap(); // ���Ƹ��º��У԰��ͼ

            dialog.accept(); // �رնԻ���
        }
        else {
            QMessageBox::warning(&dialog, QStringLiteral("����"), QStringLiteral("����д��Ч�ľ��롣"));
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
            // ���� PlaceItem�������ݻص�����
            PlaceItem* placeItem = new PlaceItem(place,
                [this](int startId, int endId) {
                    this->calculateShortestPath(startId, endId); // ���ü������·��
                });

            scene->addItem(placeItem);

            // ���������� QGraphicsTextItem ����ʾ��������
            QGraphicsTextItem* textItem = new QGraphicsTextItem(place.name); // ʹ�ó�������
            textItem->setPos(place.coordinates.first + 15, place.coordinates.second); 
            scene->addItem(textItem);
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
    Path path = campusMap.findShortestPath(startId, endId);

    QString pathResult = QStringLiteral("���·��: ");
    pathResult += QString(path.startPlace.name) + " -> " + QString(path.endPlace.name);

    highlightPath(path);
        
    QMessageBox::information(this, QStringLiteral("·�����"), pathResult);

    // �ָ�ģʽΪ�鿴����
    PlaceItem::currentMode = PlaceItem::ViewProperties; // �ָ�Ϊ�鿴����ģʽ
}

void CampusNavigation::drawPath(const Path& path) {
        // �����߶�
        QGraphicsLineItem* lineItem = new QGraphicsLineItem(path.startPlace.coordinates.first + 5,
            path.startPlace.coordinates.second + 5,
            path.endPlace.coordinates.first + 5,
            path.endPlace.coordinates.second + 5);
        lineItem->setPen(QPen(Qt::green, 2)); // ����������ɫ�Ϳ��
        scene->addItem(lineItem); // ��ӵ�������
}

void CampusNavigation::highlightPath(const Path& path) {
    // �����߶�
    QGraphicsLineItem* lineItem = new QGraphicsLineItem(path.startPlace.coordinates.first + 5,
        path.startPlace.coordinates.second + 5,
        path.endPlace.coordinates.first + 5,
        path.endPlace.coordinates.second + 5);
    lineItem->setPen(QPen(Qt::red, 2)); // ����������ɫ�Ϳ��
    scene->addItem(lineItem); // ��ӵ�������
}