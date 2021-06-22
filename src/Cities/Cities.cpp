//
// Created by gabridc on 5/6/21.
//
#include "Cities.h"
#include <QDebug>


QMap<QString, City> citiesMap_ = QMap<QString, City>();
KDTree citiesTree = KDTree();

Cities::Cities(void)
{
}

inline QString Cities::findCity(double latitude, double longitude)
{
    auto pointNear = citiesTree.nearest_point({latitude, longitude});

    for (auto c : citiesMap_)
        return c.match(pointNear[0], pointNear[1]);

    return nullptr;
}
