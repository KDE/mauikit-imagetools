//
// Created by gabridc on 5/6/21.
//

#ifndef IMAGETOOLS_CITIES_H
#define IMAGETOOLS_CITIES_H
#include <QDebug>

#include <QObject>
#include <QThread>

#include "kdtree.hpp"

#include "imagetools_export.h"

class City;
class CitiesDB;
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

    static Cities *getInstance()
    {
        qWarning() << "GETTIG CITIES INSTANCE" << QThread::currentThread();

        if (m_instance)
            return m_instance;

        m_instance = new Cities;
        return m_instance;
    }

    City *findCity(double latitude, double longitude);
    City *city(const QString&);
    
private:
    static Cities *m_instance;

    Cities(QObject * parent = nullptr);

    ~Cities();
    Cities(const Cities &) = delete;
    Cities &operator=(const Cities &) = delete;
    Cities(Cities &&) = delete;
    Cities &operator=(Cities &&) = delete;

    static KDTree *m_citiesTree;

    void parseCities();

    CitiesDB *db();
    QHash<Qt::HANDLE, CitiesDB*> m_dbs;
    
signals:
    void citiesReady();
};

#endif //IMAGETOOLS_CITIES_H
