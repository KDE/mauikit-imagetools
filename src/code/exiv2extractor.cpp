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

#include <QGeoAddress>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextCodec>

#include "geolocation/cities.h"
#include "geolocation/city.h"

Exiv2Extractor::Exiv2Extractor(const QUrl &url, QObject *parent) : QObject(parent)
, m_error(true)
, m_url(url)
{
    if (!QFileInfo::exists(m_url.toLocalFile()) || m_url.isEmpty() || !m_url.isValid()) {
        m_error = true;
    }    
    
    try {
        m_image =  Exiv2::ImageFactory::open(m_url.toLocalFile().toStdString());
    } catch (const std::exception &) {
        return;
    }
    if (!m_image.get()) {
        return;
    }
    
    if (!m_image->good()) {
        return;
    }
    
    try {
        m_image->readMetadata();
    } catch (const std::exception &) {
        return;
    }
    
    m_error = false;
}


Exiv2::ExifData & Exiv2Extractor::exifData() const
{   
    Exiv2::ExifData &exifData = m_image->exifData();
    if (exifData.empty()) {
//        qWarning() <<  "No EXIF data in : " << m_url.toString();
    }    
    
    return exifData;
}

Coordinates Exiv2Extractor::extractGPS() const
{
    double latitude = fetchGpsDouble("Exif.GPSInfo.GPSLatitude");
    double longitude = fetchGpsDouble("Exif.GPSInfo.GPSLongitude");
    
    QByteArray latRef = getExifTagData("Exif.GPSInfo.GPSLatitudeRef");
    if (!latRef.isEmpty() && latRef[0] == 'S')
        latitude *= -1;
    
    QByteArray longRef = getExifTagData("Exif.GPSInfo.GPSLongitudeRef");
    if (!longRef.isEmpty() && longRef[0] == 'W')
        longitude *= -1;
    
    return {latitude, longitude};
}

double Exiv2Extractor::fetchGpsDouble(const char *name) const
{
    Exiv2::ExifData &data = (exifData());
    Exiv2::ExifData::const_iterator it = data.findKey(Exiv2::ExifKey(name));
    if (it != data.end() && it->count() == 3) {
        double n = 0.0;
        double d = 0.0;
        
        n = (*it).toRational(0).first;
        d = (*it).toRational(0).second;
        
        if (d == 0) {
            return 0.0;
        }
        
        double deg = n / d;
        
        n = (*it).toRational(1).first;
        d = (*it).toRational(1).second;
        
        if (d == 0) {
            return deg;
        }
        
        double min = n / d;
        if (min != -1.0) {
            deg += min / 60.0;
        }
        
        n = (*it).toRational(2).first;
        d = (*it).toRational(2).second;
        
        if (d == 0) {
            return deg;
        }
        
        double sec = n / d;
        if (sec != -1.0) {
            deg += sec / 3600.0;
        }
        
        return deg;
    }
    
    return 0.0;
}

bool Exiv2Extractor::error() const
{
    return m_error;
}

QString Exiv2Extractor::getExifTagString(const char* exifTagName, bool escapeCR) const
{    
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData &data = (exifData());
        Exiv2::ExifData::iterator it = data.findKey(exifKey);
        
        
        if (it != data.end())
        {
            // See B.K.O #184156 comment #13
            std::string val  = it->print(&data);
            QString tagValue = QString::fromLocal8Bit(val.c_str());
            
            if (escapeCR)
                tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));
            
            return tagValue;
        }
    }
    catch( Exiv2::Error& e )
    {
        qWarning() << QString("Cannot find Exif key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)) << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from Exiv2";
    }
    
    return QString();
}

QByteArray Exiv2Extractor::getExifTagData(const char* exifTagName) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData &data = (exifData());        
        Exiv2::ExifData::iterator it = data.findKey(exifKey);
        
        if (it != data.end())
        {
            char* const s = new char[(*it).size()];
            (*it).copy((Exiv2::byte*)s, Exiv2::bigEndian);
            QByteArray data(s, (*it).size());
            delete[] s;
            
            return data;
        }
    }
    catch( Exiv2::Error& e )
    {
        qWarning() << QString("Cannot find Exif key '%1' into image using Exiv2 ").arg(QString::fromLatin1(exifTagName)) << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from Exiv2";
    }
    
    return QByteArray();
}

QVariant Exiv2Extractor::getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts, bool stringEscapeCR, int component) const
{
    try
    {
        Exiv2::ExifKey exifKey(exifTagName);
        Exiv2::ExifData &data = (exifData());
        Exiv2::ExifData::iterator it = data.findKey(exifKey);
        
        if (it != data.end())
        {
            switch (it->typeId())
            {
                case Exiv2::unsignedByte:
                case Exiv2::unsignedShort:
                case Exiv2::unsignedLong:
                case Exiv2::signedShort:
                case Exiv2::signedLong:
                    if (it->count() > component)
                        return QVariant((int)it->toLong(component));
                    else
                        return QVariant(QVariant::Int);
                case Exiv2::unsignedRational:
                case Exiv2::signedRational:
                    
                    if (rationalAsListOfInts)
                    {
                        if (it->count() <= component)
                            return QVariant(QVariant::List);
                        
                        QList<QVariant> list;
                        list << (*it).toRational(component).first;
                        list << (*it).toRational(component).second;
                        
                        return QVariant(list);
                    }
                    else
                    {
                        if (it->count() <= component)
                            return QVariant(QVariant::Double);
                        
                        // prefer double precision
                        double num = (*it).toRational(component).first;
                        double den = (*it).toRational(component).second;
                        
                        if (den == 0.0)
                            return QVariant(QVariant::Double);
                        
                        return QVariant(num / den);
                    }
                case Exiv2::date:
                case Exiv2::time:
                {
                    QDateTime dateTime = QDateTime::fromString(QString::fromLatin1(it->toString().c_str()), Qt::ISODate);
                    return QVariant(dateTime);
                }
                case Exiv2::asciiString:
                case Exiv2::comment:
                case Exiv2::string:
                {
                    std::ostringstream os;
                    os << *it;
                    QString tagValue = QString::fromLocal8Bit(os.str().c_str());
                    
                    if (stringEscapeCR)
                        tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));
                    
                    return QVariant(tagValue);
                }
                default:
                    break;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        qWarning () << QString("Cannot find Exif key '%1' in the image using Exiv2 ").arg(QString::fromLatin1(exifTagName)) << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from Exiv2";
    }
    
    return QVariant();
}

static bool isUtf8(const char* const buffer)
{
    int i, n;
    unsigned char c;
    bool gotone = false;
    
    if (!buffer)
        return true;
    
    // character never appears in text
    #define F 0
    // character appears in plain ASCII text
    #define T 1
    // character appears in ISO-8859 text
    #define I 2
    // character appears in non-ISO extended ASCII (Mac, IBM PC)
    #define X 3
    
    static const unsigned char text_chars[256] =
    {
        //                  BEL BS HT LF    FF CR    
        F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  // 0x0X
        //                              ESC          
        F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  // 0x1X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x2X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x3X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x4X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x5X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x6X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  // 0x7X
        //            NEL                            
        X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  // 0x8X
        X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  // 0x9X
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xaX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xbX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xcX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xdX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xeX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   // 0xfX
    };
    
    for (i = 0; (c = buffer[i]); ++i)
    {
        if ((c & 0x80) == 0)
        {
            // 0xxxxxxx is plain ASCII
            
            // Even if the whole file is valid UTF-8 sequences,
            // still reject it if it uses weird control characters.
            
            if (text_chars[c] != T)
                return false;
            
        }
        else if ((c & 0x40) == 0)
        {
            // 10xxxxxx never 1st byte
            return false;
        }
        else
        {
            // 11xxxxxx begins UTF-8
            int following = 0;
            
            if ((c & 0x20) == 0)
            {
                // 110xxxxx
                following = 1;
            }
            else if ((c & 0x10) == 0)
            {
                // 1110xxxx
                following = 2;
            }
            else if ((c & 0x08) == 0)
            {
                // 11110xxx
                following = 3;
            }
            else if ((c & 0x04) == 0)
            {
                // 111110xx
                following = 4;
            }
            else if ((c & 0x02) == 0)
            {
                // 1111110x
                following = 5;
            }
            else
            {
                return false;
            }
            
            for (n = 0; n < following; ++n)
            {
                i++;
                
                if (!(c = buffer[i]))
                    goto done;
                
                if ((c & 0x80) == 0 || (c & 0x40))
                    return false;
            }
            
            gotone = true;
        }
    }
    
    done:
    
    return gotone;   // don't claim it's UTF-8 if it's all 7-bit.
}

static QString detectEncodingAndDecode(const std::string& value) 
{
    // For charset autodetection, we could use sophisticated code
    // (Mozilla chardet, KHTML's autodetection, QTextCodec::codecForContent),
    // but that is probably too much.
    // We check for UTF8, Local encoding and ASCII.
    // Look like KEncodingDetector class can provide a full implementation for encoding detection.
    
    if (value.empty())
    {
        return QString();
    }
    
    if (isUtf8(value.c_str()))
    {
        return QString::fromUtf8(value.c_str());
    }
    
    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // So we can use either local encoding, or latin1.
    
    return QString::fromLocal8Bit(value.c_str());
}

static QString convertCommentValue(const Exiv2::Exifdatum& exifDatum)
{
    try
    {
        std::string comment;
        std::string charset;
        
        comment = exifDatum.toString();
        
        // libexiv2 will prepend "charset=\"SomeCharset\" " if charset is specified
        // Before conversion to QString, we must know the charset, so we stay with std::string for a while
        if (comment.length() > 8 && comment.substr(0, 8) == "charset=")
        {
            // the prepended charset specification is followed by a blank
            std::string::size_type pos = comment.find_first_of(' ');
            
            if (pos != std::string::npos)
            {
                // extract string between the = and the blank
                charset = comment.substr(8, pos-8);
                // get the rest of the string after the charset specification
                comment = comment.substr(pos+1);
            }
        }
        
        if (charset == "\"Unicode\"")
        {
            return QString::fromUtf8(comment.data());
        }
        else if (charset == "\"Jis\"")
        {
            QTextCodec* const codec = QTextCodec::codecForName("JIS7");
            return codec->toUnicode(comment.c_str());
        }
        else if (charset == "\"Ascii\"")
        {
            return QString::fromLatin1(comment.c_str());
        }
        else
        {
            return detectEncodingAndDecode(comment);
        }
    }
    catch( Exiv2::Error& e )
    {
        qWarning() << (QString::fromLatin1("Cannot convert Comment using Exiv2 "), e.what());
    }
    catch(...)
    {
        qWarning()<< "Default exception from Exiv2";
    }
    
    return QString();
}

MetaDataMap Exiv2Extractor::getExifTagsDataList(const QStringList& exifKeysFilter, bool invertSelection) const
{
    if (exifData().empty())
        return MetaDataMap();
    
    try
    {
        Exiv2::ExifData &data = exifData();
        data.sortByKey();
        
        MetaDataMap metaDataMap;
        
        for (Exiv2::ExifData::iterator md = data.begin(); md != data.end(); ++md)
        {
            QString key = QString::fromLatin1(md->key().c_str());
            
            // Decode the tag value with a user friendly output.
            QString tagValue;
            
            if (key == QString::fromLatin1("Exif.Photo.UserComment"))
            {
                tagValue = convertCommentValue(*md);
            }
            else if (key == QString::fromLatin1("Exif.Image.0x935c"))
            {
                tagValue = QString::number(md->value().size());
            }
            else
            {
                std::ostringstream os;
                os << *md;
                
                // Exif tag contents can be an translated strings, no only simple ascii.
                tagValue = QString::fromLocal8Bit(os.str().c_str());
            }
            
            tagValue.replace(QString::fromLatin1("\n"), QString::fromLatin1(" "));
            
            // We apply a filter to get only the Exif tags that we need.
            
            if (!exifKeysFilter.isEmpty())
            {
                if (!invertSelection)
                {
                    if (exifKeysFilter.contains(key.section(QString::fromLatin1("."), 1, 1)))
                        metaDataMap.insert(key, tagValue);
                }
                else
                {
                    if (!exifKeysFilter.contains(key.section(QString::fromLatin1("."), 1, 1)))
                        metaDataMap.insert(key, tagValue);
                }
            }
            else // else no filter at all.
            {
                metaDataMap.insert(key, tagValue);
            }
        }
        
        return metaDataMap;
    }
    catch (Exiv2::Error& e)
    {
        qWarning() << (QString::fromLatin1("Cannot parse EXIF metadata using Exiv2 "), e.what());
    }
    catch(...)
    {
        qWarning() << "Default exception from Exiv2";
    }
    
    return MetaDataMap();
}

QString Exiv2Extractor::getExifComment() const
{
    try
    {
        if (!exifData().empty())
        {
            Exiv2::ExifData &data(exifData());
            Exiv2::ExifKey key("Exif.Photo.UserComment");
            Exiv2::ExifData::iterator it = data.findKey(key);
            
            if (it != data.end())
            {
                QString exifComment = convertCommentValue(*it);
                
                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !exifComment.trimmed().isEmpty())
                    return exifComment;
            }
            
            Exiv2::ExifKey key2("Exif.Image.ImageDescription");
            Exiv2::ExifData::iterator it2 = data.findKey(key2);
            
            if (it2 != data.end())
            {
                QString exifComment = convertCommentValue(*it2);
                
                // Some cameras fill in nonsense default values
                QStringList blackList;
                blackList << QString::fromLatin1("SONY DSC"); // + whitespace
                blackList << QString::fromLatin1("OLYMPUS DIGITAL CAMERA");
                blackList << QString::fromLatin1("MINOLTA DIGITAL CAMERA");
                
                QString trimmedComment = exifComment.trimmed();
                
                // some cameras fill the UserComment with whitespace
                if (!exifComment.isEmpty() && !trimmedComment.isEmpty() && !blackList.contains(trimmedComment))
                    return exifComment;
            }
        }
    }
    catch( Exiv2::Error& e )
    {
        qWarning() << (QString::fromLatin1("Cannot find Exif User Comment using Exiv2 "), e.what());
    }
    catch(...)
    {
        qWarning() << "Default exception from Exiv2";
    }
    
    return QString();
}

QString Exiv2Extractor::GPSString() const
{
    if(error())
    {
        return QString();
    }

    std::unique_ptr<City>m_city(city());

    if(!m_city)
    {
        return QString();
    }

    if(!m_city->isValid())
    {
        return QString();
    }
    
    return m_city->name();
}

QString Exiv2Extractor::cityId() const
{
    if(error())
    {
        return QString();
    }
    
    std::unique_ptr<City>m_city(city());

    if(!m_city)
    {
        return QString();
    }

    if(!m_city->isValid())
    {
        return QString();
    }
    
    return m_city->id();
}

City* Exiv2Extractor::city() const
{
    if(error())
    {
        return nullptr;
    }
    
    auto c = extractGPS();
    
    if(c.first == 0.0 || c.second == 0.0)
    {
        return nullptr;
    }
    
    return Cities::getInstance()->findCity(c.first, c.second);
}
