// SPDX-License-Identifier: LGPL-3.0-or-later

/*
    Copyright (C) 2012-15  Vishesh Handa <vhanda@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <QString>
#include <QVariant>
#include <QUrl>
#include <QSize>

#include "imagetools_export.h"

typedef QMap<QString, QString> MetaDataMap;
struct Coordinates
{
    double altitude;
    double latitude;
    double longitude;
};

namespace  KExiv2Iface
{
class KExiv2;
}
class City;

/**
 * @brief Image metadata extractor. 
 */
class IMAGETOOLS_EXPORT Exiv2Extractor : public QObject
{
public:
    Exiv2Extractor(const QUrl &url, QObject * parent = nullptr);
    explicit Exiv2Extractor(QObject * parent = nullptr);

    ~Exiv2Extractor();
    
    void setUrl(const QUrl &url);
    
    Coordinates extractGPS() const;

    bool error() const;
    
    QString getExifTagString (const char *exifTagName, bool escapeCR=true) const;
    QByteArray getExifTagData (const char *exifTagName) const;
    QVariant getExifTagVariant (const char *exifTagName, bool rationalAsListOfInts=true, bool escapeCR=true, int component=0) const;
    MetaDataMap getExifTagsDataList( const QStringList & exifKeysFilter = QStringList(), bool invertSelection = false ) const;
    
    bool writeTag(const char *tagName, const QVariant &value);
    bool removeTag(const char *tagName);
    
    bool setComment(const QString &comment);
    QString getComments() const;
    QString getExifComment() const;
    bool removeComment() const;

    QString GPSString() const;
    QString cityId() const;
    City city() const;

    QSize getPixelSize();
    bool applyChanges();

    bool setGpsData(const double latitude, const double longitude, const double altitude = 0.0);
    bool removeGpsData();

    bool clearData();

private:

    bool m_error;
    
    QUrl m_url;
    QString m_mimetype;
    KExiv2Iface::KExiv2 *m_exiv2;
};
