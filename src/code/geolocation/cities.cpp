//
// Created by gabridc on 5/6/21.
//
#include "cities.h"

#include <QStringList>

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

#include <QUuid>

//Cities QMap<Qt:HANDLE, Cities*>::m_instances = {};

Cities::Cities(QObject * parent) : QObject(parent)
{
    qDebug() << "Setting up Cities instance";

    if(QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")))
    {
        qDebug() << "opening Cities DB";
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QUuid::createUuid().toString());

        m_db.setDatabaseName("/home/camilo/Coding/qml/mauiimageviewer/src/data/cities.db");
        qDebug() << "Cities DB NAME" << m_db.connectionName();

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
    if(Cities::m_pointVector.empty())
    {
        qDebug() << "KDE TREE EMPTY FILLING IT";
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
            Cities::m_pointVector.push_back({lat, lon});
        }

        Cities::m_citiesTree = KDTree(Cities::m_pointVector);
        m_error = false;
        emit citiesReady();
    }else
    {
        m_error = false;
    }

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
