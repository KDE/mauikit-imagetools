//
// Created by gabridc on 5/6/21.
//

#pragma once
#include <QDebug>

#include <QObject>
#include <QThread>

#include "kdtree.hpp"

#include "imagetools_export.h"

class City;
class CitiesDB;
class IMAGETOOLS_EXPORT Cities : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Cities)

public:
    Cities(QObject * parent = nullptr);

    static Cities *getInstance();

    ~Cities();
    City findCity(double latitude, double longitude);
    City city(const QString&);
    
private:
    CitiesDB *db();
    QHash<Qt::HANDLE, CitiesDB*> m_dbs;
};

