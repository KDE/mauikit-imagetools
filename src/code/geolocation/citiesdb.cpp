#include "citiesdb.h"

#include "city.h"
#include <QDebug>

#include <QFile>
#include <QDir>

#include <QStandardPaths>
#include <QUuid>

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>


static QString resolveDBFile()
{
#if defined(Q_OS_ANDROID)

    QFile file(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "/org/mauikit/imagetools/cities.db"));
    if(!file.exists())
    {
        if(QFile::copy(":/android_rcc_bundle/qml/org/mauikit/imagetools/cities.db", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/cities.db"))
        {
            qDebug() << "Cities DB File was copied to";
        }else
        {
            qDebug() << "Cities DB File was NOT copied to";

        }
    }
    return  QStandardPaths::locate(QStandardPaths::GenericDataLocation, "cities.db");
#else
    return QStandardPaths::locate(QStandardPaths::GenericDataLocation, "/org/mauikit/imagetools/cities.db");
#endif
}

CitiesDB::CitiesDB(QObject * parent) : QObject(parent)
{
    QString DBFile = resolveDBFile();

    if(QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE")))
    {
        qDebug() << "opening Cities DB";
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QUuid::createUuid().toString());

        m_db.setDatabaseName(DBFile);
        qDebug() << "Cities DB NAME" << m_db.connectionName();
        qDebug() << DBFile;

        if(!m_db.open())
        {
            qWarning() << "Cities::DatabaseConnect - ERROR: " << m_db.lastError().text();
            m_error = true;
        }else
        {
            m_error = false;
        }
    }
    else
    {
        qWarning() << "Cities::DatabaseConnect - ERROR: no driver " << QStringLiteral("QSQLITE") << " available";
        m_error = true;
    }
}

City CitiesDB::findCity(double latitude, double longitude)
{
    if(m_error)
    {
        return City();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM CITIES where lat = ? and lon = ?");
    query.addBindValue(latitude);
    query.addBindValue(longitude);

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

City CitiesDB::city(const QString &cityId)
{
    if(m_error)
    {
        return City();
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT c.id, c.name, co.name as country, c.lat, c.lon, c.tz FROM CITIES c inner join COUNTRIES co on c.country = co.id where c.id = ?");
    query.addBindValue(cityId);

    if(!query.exec())
    {
        qWarning() << "Cities::city - ERROR: " << query.lastError().text();
    }

    if(query.first())
    {
        return City(query.value("id").toString(), query.value("name").toString(), query.value("tz").toString(), query.value("country").toString(),query.value("lat").toDouble(),query.value("lon").toDouble());
    }

    return City();
}

std::vector< point_t > CitiesDB::cities()
{
    std::vector< point_t >  res;
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
        res.push_back({lat, lon});
    }

     m_error = false;
    return res;
}


bool CitiesDB::error() const
{
    return m_error;
}

