//
// Created by gabridc on 5/6/21.
//
#include "cities.h"
#include <QDebug>

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QCoreApplication>

#include "city.h"

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
    
    parseCities(); //this maybe shoudl be in a different thread and hook to a signal when the file has been parsed.
}

bool Cities::error() const
{
    return m_error;
}

void Cities::parseCities()
{
    
    QFile file("/home/camilo/coding/mauiimageviewer/src/code/geolocation/cities1000.txt"); // this file shuld be installed in a common path to later be accessed easily, putting it on qrc is not a good idea
    qDebug() << "Loading cities files" << file.exists();
    
    pointVec pointVector;
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() <<  "Failed to open cities1000.txt file";
        m_error = true;
        return;
    }
    
    QTextStream fstream(&file);
    
    while (!fstream.atEnd()) 
    {
        QString str = fstream.readLine();
        str.remove('\r');
        
        QStringList list = str.split('\t');
        QString cityID = list[0];
        QString name = list[1];
        double lat = list[4].toDouble();
        double lon = list[5].toDouble();
        QString countryPrefix = list[8];
        QString continent = list[17];
        
        m_citiesMap.insert(cityID, new City{cityID, name, continent, countryPrefix, lat, lon, this});
        pointVector.push_back({lat, lon});
    }
    
    m_citiesTree = KDTree(pointVector);
    m_error = false;
    emit citiesReady(); 
}

const City* Cities::findCity(double latitude, double longitude)
{
    if(m_error)
    {
        return new City(this);
    }
    
    qDebug() << "Latitude: " << latitude << "Longitud: " << longitude;
    auto pointNear = m_citiesTree.nearest_point({latitude, longitude});
    
    for (const auto &c : m_citiesMap)
    {
        if(c->match(pointNear[0], pointNear[1]))
        {
            return c;
        }
    }
    
    return new City(this);
}

const City * Cities::city(const QString &cityId)
{
    if(m_error)
    {
        return new City(this);
    }
    
    if(m_citiesMap.contains(cityId))
    {
        return m_citiesMap[cityId];
    }
    
    return new City(this);
}
