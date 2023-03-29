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
    
public:

    static Cities *getInstance()
    {
        static Cities instance;
        return &instance;
    }
~Cities();
    City findCity(double latitude, double longitude);
    City city(const QString&);
    
private:
    Cities(QObject * parent = nullptr);

    Cities(const Cities &) = delete;
    Cities &operator=(const Cities &) = delete;
    Cities(Cities &&) = delete;
    Cities &operator=(Cities &&) = delete;
    
    CitiesDB *db();
    QHash<Qt::HANDLE, CitiesDB*> m_dbs;
Q_SIGNALS:
};

