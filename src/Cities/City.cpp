//
// Created by gabridc on 5/6/21.
//
#include "City.h"




QString City::match(double latitude, double longitude)
{
    if(latitude_ == latitude && longitude_ == longitude)
        return cityID_;
    else
        return nullptr;
}