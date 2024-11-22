// Place.h
#ifndef PLACE_H
#define PLACE_H

#include <QString>
#include <utility>

struct Place {
    int id; // ���
    QString name; // ����
    std::pair<double, double> coordinates; // ����
    QString type; // ���
};

#endif // PLACE_H

