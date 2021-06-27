//
// Created by gabridc on 5/6/21.
//
#include "cities.h"
#include <QDebug>

#include <QStringList>
#include <QCoreApplication>

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>


Cities *Cities::m_instance = nullptr;

Cities::Cities(QObject * parent) : QObject(parent)
{
    qDebug() << "Setting up Cities instance" << m_instance;
    
    connect(qApp, &QCoreApplication::aboutToQuit, []()
    {
        qDebug() << "Lets remove Cities singleton instance";
        delete m_instance;
        m_instance = nullptr;
    });
    

    if(QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")))
    {
        qDebug() << "opening Cities DB";
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), "MauiKit::ImageTools::Cities");

        m_db.setDatabaseName("/home/camilo/Coding/qml/mauiimageviewer/src/data/cities.db");

        if(!m_db.open())
        {
            qWarning() << "Cities::DatabaseConnect - ERROR: " << m_db.lastError().text();
            m_error = true;
            return;
        }
    }
    else
    {
        qWarning() << "Cities::DatabaseConnect - ERROR: no driver " << QStringLiteral("QSQLITE") << " available";
        m_error = true;
        return;
    }

    m_error = false;
    parseCities();
}

bool Cities::error() const
{
    return m_error;
}

void Cities::parseCities()
{    
    pointVec pointVector;

    QSqlQuery query(m_db);
    query.prepare("SELECT lat, lon FROM CITIES");

    if(!query.exec())
    {
        qWarning() << "Cities::ParsingCities - ERROR: " << query.lastError().text();
        m_error = true;
    }

    while(query.next())
    {
        double lat = query.value("lat").toDouble();
        double lon = query.value("lon").toDouble();
        pointVector.push_back({lat, lon});
    }
    
    m_citiesTree = KDTree(pointVector);
    m_error = false;
    emit citiesReady();
}

const City Cities::findCity(double latitude, double longitude)
{
    if(m_error)
    {
        return City();
    }
    
    qDebug() << "Latitude: " << latitude << "Longitud: " << longitude;
    auto pointNear = m_citiesTree.nearest_point({latitude, longitude});

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM CITIES where lat = ? and lon = ?");
    query.addBindValue(pointNear[0]);
    query.addBindValue(pointNear[1]);

    qDebug() << query.lastQuery() << pointNear[0] << pointNear[1];

    if(!query.exec())
    {
        qWarning() << "Cities::FindCity - ERROR: " << query.lastError().text();
    }

    if(query.first())
    {
        return City(query.value("id").toString(), query.value("name").toString(), query.value("tz").toString(), query.value("country").toString(),query.value("lat").toDouble(),query.value("lon").toDouble());
    }

    qWarning() << "City not found";

    return City();
}

const City Cities::city(const QString &cityId)
{
    if(m_error)
    {
        return City();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM CITIES where id = ?");
    query.addBindValue(cityId);

    if(!query.exec())
    {
        qWarning() << "Cities::city - ERROR: " << query.lastError().text();
    }

    if(query.first())
    {
        return City(query.value("id").toString(), query.value("name").toString(), query.value("tz").toString(), query.value("country").toString(),query.value("lat").toDouble(),query.value("lon").toDouble(), this);
    }

    return City();
}
