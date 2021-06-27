//
// Created by gabridc on 5/6/21.
//

#ifndef IMAGETOOLS_CITIES_H
#define IMAGETOOLS_CITIES_H
#include <QDebug>

#include <QObject>
#include <QSqlDatabase>

#include "kdtree.hpp"

#include "imagetools_export.h"

#include "city.h"

class KDTree_Cities
{

public:
    KDTree_Cities *instance()
    {
        static KDTree_Cities instance;
        return &instance;
    }

private:
    KDTree_Cities();

};

class IMAGETOOLS_EXPORT Cities : public QObject
{
    Q_OBJECT
    
public:
    Cities(QObject * parent = nullptr);
    
    const City findCity(double latitude, double longitude);
    const City city(const QString&);
    
    bool error() const;
    
private:
    inline static pointVec m_pointVector;
    inline static KDTree m_citiesTree;

    bool m_error = {true};
    QSqlDatabase m_db;
    void parseCities();
    
signals:
    void citiesReady();
};

#endif //IMAGETOOLS_CITIES_H
