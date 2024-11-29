// Place.h
#ifndef PATH_H
#define PATH_H

#include <QString>
#include <utility>
#include "Place.h"

class Path {
public:
    Place startPlace;
    Place endPlace;
    double distance; // Â·¾¶µÄ×Ü¾àÀë

    Path(const Place& start, const Place& end, double dist)
        : startPlace(start), endPlace(end), distance(dist) {}
};



#endif