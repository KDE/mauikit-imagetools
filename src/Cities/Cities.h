//
// Created by gabridc on 5/6/21.
//

#ifndef IMAGETOOLS_CITIES_H
#define IMAGETOOLS_CITIES_H
#include <QFile>
#include <QMap>
#include <qtextstream.h>
#include <qstringlist.h>
#include "KDTree.hpp"
#include "City.h"


class Cities{

public:
    static Cities *instance()
    {
        static Cities cities;

        QFile file("cities1000.txt");

        pointVec pointVector;

        if (!file.open(QIODevice::ReadOnly)) {
            Q_ASSERT_X(0, "", "Failed to open cities1000.txt file");
        }

        QTextStream fstream(&file);
        while (!fstream.atEnd()) {
            QString str = fstream.readLine();
            str.remove('\r');

            QStringList list = str.split('\t');
            QString cityID = list[0];
            QString poblation = list[1];
            double lat = list[4].toDouble();
            double lon = list[5].toDouble();
            QString countryPrefix = list[8];
            QString continent = list[17];

            cities.citiesMap_.insert(cityID, {cityID, continent, countryPrefix, lat, lon});
            pointVector.push_back({lat, lon});
        }

        cities.citiesTree = KDTree(pointVector);

        return &cities;
    }

    Cities(void);

    Cities(const Cities &) = delete;
    Cities &operator=(const Cities &) = delete;
    Cities(Cities &&) = delete;
    Cities &operator=(Cities &&) = delete;


    inline static QString findCity(double latitude, double longitude);

private:


private:
    static QMap<QString, City> citiesMap_;
    static KDTree citiesTree;


};



#endif //IMAGETOOLS_CITIES_H
