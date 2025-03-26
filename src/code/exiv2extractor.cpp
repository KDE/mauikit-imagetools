// SPDX-License-Identifier: LGPL-3.0-or-later

/*
 *    Copyright (C) 2012-15  Vishesh Handa <vhanda@kde.org>
 * 
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 * 
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 * 
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "exiv2extractor.h"

#include <KExiv2/KExiv2>

#include <QGeoAddress>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>

// #include <QTextCodec>

#include "geolocation/cities.h"
#include "geolocation/city.h"

Exiv2Extractor::Exiv2Extractor(const QUrl &url, QObject *parent) : QObject(parent)
    , m_error(true)
    , m_exiv2(new KExiv2Iface::KExiv2()) //remeember to delete it
{
    if(!KExiv2Iface::KExiv2::initializeExiv2())
        qWarning() << "failed to initialized exiv2 library";
    this->setUrl(url);
}

Exiv2Extractor::Exiv2Extractor(QObject *parent) : QObject(parent)
    , m_error(true)
    , m_exiv2(new KExiv2Iface::KExiv2()) //remeember to delete it

{
}

Exiv2Extractor::~Exiv2Extractor()
{
    delete m_exiv2;
}

void Exiv2Extractor::setUrl(const QUrl &url)
{
    qDebug() << "Start parsing image file url for metadata";
    m_url = url;
    if (!QFileInfo::exists(m_url.toLocalFile()) || m_url.isEmpty() || !m_url.isValid()) {
        qDebug() << "Image file is not valid or does not exists.";
        return;
    }
    
    m_error = !m_exiv2->load(m_url.toLocalFile());
}

Coordinates Exiv2Extractor::extractGPS() const
{
    if(!m_exiv2->initializeGPSInfo(true))
    {
        qWarning() << "failed to initialized GPS data";
        return {};
    }

    double latitude = 0;
    double longitude = 0;
    double altitude = 0;

    m_exiv2->getGPSLatitudeNumber(&latitude);
    m_exiv2->getGPSLongitudeNumber(&longitude);
    m_exiv2->getGPSAltitude(&altitude);

    return {altitude, latitude, longitude};
}


bool Exiv2Extractor::error() const
{
    return m_error;
}

QString Exiv2Extractor::getExifTagString(const char* exifTagName, bool escapeCR) const
{
    return m_exiv2->getExifTagString(exifTagName, escapeCR);
}

QByteArray Exiv2Extractor::getExifTagData(const char* exifTagName) const
{  
    return m_exiv2->getExifTagData(exifTagName);
}

QVariant Exiv2Extractor::getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts, bool stringEscapeCR, int component) const
{
    return m_exiv2->getExifTagVariant(exifTagName, rationalAsListOfInts, stringEscapeCR, component);
}

MetaDataMap Exiv2Extractor::getExifTagsDataList(const QStringList& exifKeysFilter, bool invertSelection) const
{
    return m_exiv2->getExifTagsDataList(exifKeysFilter, invertSelection);
}

QString Exiv2Extractor::getExifComment() const
{
    
    return m_exiv2->getExifComment();
}

QString Exiv2Extractor::GPSString() const
{
    if(error())
    {
        return QString();
    }
    
    City m_city(city());

    if(!m_city.isValid())
    {
        return QString();
    }
    
    return m_city.name();
}

QString Exiv2Extractor::cityId() const
{
    if(error())
    {
        return QString();
    }   

    return city().id();
}

City Exiv2Extractor::city() const
{
    if(error())
    {
        return City();
    }
    
    auto c = extractGPS();
    
    if(c.latitude == 0.0 || c.longitude == 0.0)
    {
        return City();
    }
    
    return Cities::getInstance()->findCity(c.latitude, c.longitude);
}

bool Exiv2Extractor::applyChanges()
{
    return m_exiv2->applyChanges();
}

bool Exiv2Extractor::setGpsData(const double latitude, const double longitude, const double altitude)
{
    qDebug() << "Setting gps data as:" << latitude << longitude << altitude;
    return m_exiv2->setGPSInfo(altitude,latitude, longitude);

    // return m_exiv2->setGPSInfo(0.0, 6.224958, -75.573983);
}

bool Exiv2Extractor::removeGpsData()
{
    return m_exiv2->removeGPSInfo();
}

bool Exiv2Extractor::writeTag(const char *tagName, const QVariant &value)
{
    return m_exiv2->setExifTagVariant(tagName, value);
}

bool Exiv2Extractor::removeTag(const char *tagName)
{
    return m_exiv2->removeExifTag(tagName);
}

bool Exiv2Extractor::setComment(const QString &comment)
{
    return m_exiv2->setExifComment(comment);
}

QString Exiv2Extractor::getComments() const
{
    return m_exiv2->getCommentsDecoded();
}
