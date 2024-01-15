//
// Created by gabridc on 5/6/21.
//
#include "cities.h"

#include <QStringList>

#include <QCoreApplication>

#include "city.h"
#include "citiesdb.h"

Cities *Cities::m_instance = nullptr;
KDTree *Cities::m_citiesTree = nullptr;

Cities::Cities(QObject * parent) : QObject(parent)
{
    qDebug() << "Setting up Cities instance";

    connect(qApp, &QCoreApplication::aboutToQuit, [this]()
    {
        qDebug() << "Lets remove Tagging singleton instance";

        qDeleteAll(m_dbs);
        m_dbs.clear();

        delete m_instance;
        m_instance = nullptr;
    });

    parseCities();
}

Cities::~Cities()
{
    delete Cities::m_citiesTree;
}

City* Cities::findCity(double latitude, double longitude)
{
    qDebug() << "Latitude: " << latitude << "Longitud: " << longitude;
    auto pointNear = Cities::m_citiesTree->nearest_point({latitude, longitude});
    qDebug()  << pointNear[0] << pointNear[1];

   return db()->findCity(pointNear[0], pointNear[1]);
}

City *Cities::city(const QString &id)
{
    return db()->city(id);
}

void Cities::parseCities()
{    
    if(!Cities::m_citiesTree || Cities::m_citiesTree->empty())
    {
        qDebug() << "KDE TREE EMPTY FILLING IT";

        Cities::m_citiesTree = new KDTree(db()->cities());
        Q_EMIT citiesReady();
    }
}

CitiesDB *Cities::db()
{
    if(m_dbs.contains(QThread::currentThreadId()))
    {
        qDebug() << "Using existing CITIESDB instance" << QThread::currentThreadId();

        return m_dbs[QThread::currentThreadId()];
    }

    qDebug() << "Creating new CITIESDB instance" << QThread::currentThreadId();

    auto new_db = new CitiesDB;
    m_dbs.insert(QThread::currentThreadId(), new_db);
    return new_db;
}
