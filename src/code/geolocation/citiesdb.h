#pragma once

#include <QObject>
#include <QSqlDatabase>
#include "kdtree.hpp"

class City;
class CitiesDB : public QObject
{
    Q_OBJECT

public:
    explicit CitiesDB(QObject * =nullptr);

    City findCity(double latitude, double longitude);
    City city(const QString&);
    std::vector<point_t> cities();
    bool error() const;

private:
    QSqlDatabase m_db;
    bool m_error = {true};
};
