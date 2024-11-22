// Place.h
#ifndef PLACE_H
#define PLACE_H

#include <QString>
#include <utility>

struct Place {
    int id; // 编号
    QString name; // 名称
    std::pair<double, double> coordinates; // 坐标
    QString type; // 类别
};

#endif // PLACE_H

