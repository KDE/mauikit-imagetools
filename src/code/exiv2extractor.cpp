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
    // if(!KExiv2Iface::KExiv2::initializeExiv2())
        // qWarning() << "failed to initialized exiv2 library";
    this->setUrl(url);
}

Exiv2Extractor::Exiv2Extractor(QObject *parent) : QObject(parent)
    , m_error(true)
    , m_exiv2(new KExiv2Iface::KExiv2()) //remeember to delete it

{
}

Exiv2Extractor::~Exiv2Extractor()
{
    // clearData();
    delete m_exiv2;
}

void Exiv2Extractor::setUrl(const QUrl &url)
{
    qDebug() << "Start parsing image file url for metadata";
    // clearData();
    m_url = url;
    if (!QFileInfo::exists(m_url.toLocalFile()) || m_url.isEmpty() || !m_url.isValid()) {
        qDebug() << "Image file is not valid or does not exists.";
        return;
    }
    
    m_error = !m_exiv2->load(m_url.toLocalFile());
    if(m_error)
        qWarning() << "Failed to load Exiv2 metadata";
}

Coordinates Exiv2Extractor::extractGPS() const
{
    if(error())
        return{};

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
    if(error())
        return {};

    return m_exiv2->getExifTagString(exifTagName, escapeCR);
}

QByteArray Exiv2Extractor::getExifTagData(const char* exifTagName) const
{
    if(error())
        return {};

    return m_exiv2->getExifTagData(exifTagName);
}

QVariant Exiv2Extractor::getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts, bool stringEscapeCR, int component) const
{
    if(error())
        return {};
    return m_exiv2->getExifTagVariant(exifTagName, rationalAsListOfInts, stringEscapeCR, component);
}

MetaDataMap Exiv2Extractor::getExifTagsDataList(const QStringList& exifKeysFilter, bool invertSelection) const
{
    if(error())
        return {};
    return m_exiv2->getExifTagsDataList(exifKeysFilter, invertSelection);
}

QString Exiv2Extractor::getExifComment() const
{
    if(error())
        return {};
    return m_exiv2->getExifComment();
}

QString Exiv2Extractor::GPSString() const
{
    if(error())
        return {};
    
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
        return {};

    return city().id();
}

City Exiv2Extractor::city() const
{
    if(error())
        return City{};

    auto c = extractGPS();
    
    if(c.latitude == 0.0 || c.longitude == 0.0)
    {
        return City();
    }
    
    return Cities::getInstance()->findCity(c.latitude, c.longitude);
}

QSize Exiv2Extractor::getPixelSize()
{
    if(error())
        return {};

    return m_exiv2->getPixelSize();
}

bool Exiv2Extractor::applyChanges()
{
    if(error())
        return false;
    return m_exiv2->applyChanges();
}

bool Exiv2Extractor::setGpsData(const double latitude, const double longitude, const double altitude)
{
    if(error())
        return false;
    qDebug() << "Setting gps data as:" << latitude << longitude << altitude;
    return m_exiv2->setGPSInfo(altitude,latitude, longitude);

           // return m_exiv2->setGPSInfo(0.0, 6.224958, -75.573983);
}

bool Exiv2Extractor::removeGpsData()
{
    if(error())
        return false;
    return m_exiv2->removeGPSInfo();
}

bool Exiv2Extractor::clearData()
{
    if(error())
        return false;

    if(m_exiv2->isEmpty())
        return false;

    auto ok = KExiv2Iface::KExiv2::cleanupExiv2 ( );
    ok = m_exiv2->clearExif ( );
    return ok;
}

bool Exiv2Extractor::writeTag(const char *tagName, const QVariant &value)
{
    if(error())
        return false;
    return m_exiv2->setExifTagVariant(tagName, value);
}

bool Exiv2Extractor::removeTag(const char *tagName)
{
    if(error())
        return false;
    return m_exiv2->removeExifTag(tagName);
}

bool Exiv2Extractor::setComment(const QString &comment)
{
    if(error())
        return false;
    return m_exiv2->setExifComment(comment);
}

QString Exiv2Extractor::getComments() const
{
    if(error())
        return {};
    return m_exiv2->getCommentsDecoded();
}

bool Exiv2Extractor::removeComment() const
{
    if(error())
        return false;
    auto ok = m_exiv2->removeExifTag("Exif.Image.ImageDescription");
    ok = m_exiv2->removeExifTag("Exif.Photo.UserComment");
    return ok;
}
