//
// Created by gabridc on 5/6/21.
//

#ifndef PIX_CITY_H
#define PIX_CITY_H

#include <QObject>
#include <QString>

#include "imagetools_export.h"

class IMAGETOOLS_EXPORT City : public QObject
{
    Q_OBJECT
    
public:
    explicit City(const QString &cityID, const QString &name, const QString &continent, const QString &country, const double &latitude, const double &longitude, QObject *parent = nullptr);
    
    explicit City(QObject *parent = nullptr);
    
    bool match(double latitude, double longitude);
    
    int operator==(City c)
    {
        if(m_cityID == c.m_cityID)
            return 1;
        else
            return 0;
    }
    
    QString id() const;
    QString name() const;
    QString country() const;
    QString continent() const;
    
    bool isValid() const;
    
private:
    QString m_cityID;
    QString m_name;
    QString m_continent;
    QString m_country;
    double m_latitude;
    double m_longitude;
};

#endif //PIX_CITY_H
