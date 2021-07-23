#ifndef CITIESDB_H
#define CITIESDB_H

#include <QObject>
#include <QSqlDatabase>
#include "kdtree.hpp"

class City;
class CitiesDB : public QObject
{
    Q_OBJECT

public:
    explicit CitiesDB(QObject * =nullptr);

    const City findCity(double latitude, double longitude);
    const City city(const QString&);
    std::vector<point_t> cities();
    bool error() const;

private:
    QSqlDatabase m_db;
    bool m_error = {true};

};

#endif // CITIESDB_H
