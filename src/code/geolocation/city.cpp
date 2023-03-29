//
// Created by gabridc on 5/6/21.
//
#include "city.h"

City::City(const QString &cityID, const QString &name, const QString &continent, const QString &country, const double &latitude, const double &longitude, QObject *parent) : QObject(parent)
, m_cityID(cityID)
, m_name(name)
, m_continent(continent)
, m_country(country)
, m_latitude(latitude)
, m_longitude(longitude)

{
    
}

City::City(const City &other, QObject *parent) : QObject(parent)
, m_cityID(other.id())
, m_name(other.name())
, m_continent(other.continent())
, m_country(other.country())
, m_latitude(other.latitude())
, m_longitude(other.longitude())

{
    
}


City::City(QObject* parent) : QObject(parent)
{
}

double City::latitude() const
{
    return m_latitude;
}

double City::longitude() const
{
    return m_longitude;
}

QString City::id() const
{
    return m_cityID;
}

QString City::name() const
{
    return m_name;
}

QString City::continent() const
{
    return m_continent;
}

QString City::country() const
{
    return m_country;
}

bool City::isValid() const
{
return !m_cityID.isEmpty();    
}

bool City::match(double latitude, double longitude)
{
    if(m_latitude == latitude && m_longitude == longitude)
    {
        return true;
    }
    
    return false;
}
