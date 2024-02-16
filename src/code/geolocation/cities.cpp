//
// Created by gabridc on 5/6/21.
//
#include "cities.h"

#include <QStringList>

#include <QCoreApplication>

#include "city.h"
#include "citiesdb.h"

static KDTree& getCitiesTree()
{
    static KDTree tree((CitiesDB()).cities());
    return tree;
}

Q_GLOBAL_STATIC(Cities, citiesInstance)

Cities::Cities(QObject * parent) : QObject(parent)
{
    qDebug() << "Setting up Cities instance";
}

Cities *Cities::getInstance()
{
    return citiesInstance();
}

Cities::~Cities()
{
    qDeleteAll(m_dbs);
    m_dbs.clear();
}

City Cities::findCity(double latitude, double longitude)
{
    // CitiesDB db;
    auto pointNear = getCitiesTree().nearest_point({latitude, longitude});
    return db()->findCity(pointNear[0], pointNear[1]);
}

City Cities::city(const QString &id)
{
    // CitiesDB db;
    return db()->city(id);
}

CitiesDB *Cities::db()
{
    if(m_dbs.contains(QThread::currentThread()))
    {
        qDebug() << "Using existing CITIESDB instance" << QThread::currentThread();
        
        return m_dbs[QThread::currentThread()];
    }
    
    qDebug() << "Creating new CITIESDB instance" << QThread::currentThread();
    
    auto new_db = new CitiesDB;
    m_dbs.insert(QThread::currentThread(), new_db);
    return new_db;
}

