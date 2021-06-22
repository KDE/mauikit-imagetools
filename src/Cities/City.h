//
// Created by gabridc on 5/6/21.
//

#ifndef PIX_CITY_H
#define PIX_CITY_H
#include <QString>


class City {

public:
    City(void) = delete;
    City(QString cityID, QString continent, QString country, double latitude, double longitude):
        cityID_(cityID), continent_(continent), country_(country), latitude_(latitude), longitude_(longitude){};

    int operator==(City c)
    {
        if(cityID_ == c.cityID_)
            return 1;
        else
            return 0;
    }

    QString match(double latitude, double longitude);



private:
    QString cityID_;
    QString continent_;
    QString country_;
    double latitude_, longitude_;





};




#endif //PIX_CITY_H
