//
// Created by gabridc on 5/6/21.
//

#ifndef IMAGETOOLS_CITIES_H
#define IMAGETOOLS_CITIES_H

#include <QObject>
#include <QMap>

#include "kdtree.hpp"
#include "city.h"

#include "imagetools_export.h"

class IMAGETOOLS_EXPORT Cities : public QObject
{
Q_OBJECT

public:
    static Cities *instance()
    {
        if(m_instance)
        {
            return m_instance;
        }
        
        m_instance = new Cities();
        return m_instance;
    }
 
    Cities(const Cities &) = delete;
    Cities &operator=(const Cities &) = delete;
    Cities(Cities &&) = delete;
    Cities &operator=(Cities &&) = delete;

    QString findCity(double latitude, double longitude);

private:
    static Cities *m_instance;
    Cities(QObject * parent = nullptr);
    QMap<QString, City*> m_citiesMap;
    KDTree m_citiesTree;
    
    void parseCities();

signals:
    void citiesReady();
};



#endif //IMAGETOOLS_CITIES_H
