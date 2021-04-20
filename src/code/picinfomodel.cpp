// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "picinfomodel.h"
#include "exiv2extractor.h"
//#include "utils/picinfo/reversegeocoder.h"
#include <QGeoAddress>
#include <QDateTime>
#include <QDebug>

std::string gpsToString(Exiv2::Metadatum& value);
PicInfoModel::PicInfoModel(QObject *parent)
    : MauiList(parent)
//  , m_geoCoder( new ReverseGeoCoder)
{
}

PicInfoModel::~PicInfoModel()
{
    //    delete m_geoCoder;
}

void PicInfoModel::componentComplete()
{
    connect(this, &PicInfoModel::urlChanged, this, &PicInfoModel::parse);
    this->parse();
}

void PicInfoModel::setUrl(QUrl url)
{        
    if (m_url == url)
    {
        return;
    }
    
    m_url = url;
    emit urlChanged(m_url);   
}

QUrl PicInfoModel::url() const
{
    return m_url;    
}

static FMH::MODEL_LIST basicInfo(const QUrl &url)
{
    FMH::MODEL_LIST res;
    QFileInfo file(url.toLocalFile());

    res << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Name"}, {FMH::MODEL_KEY::VALUE, file.fileName()}, {FMH::MODEL_KEY::ICON, "edit-rename"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Path"}, {FMH::MODEL_KEY::VALUE, url.toLocalFile()}, {FMH::MODEL_KEY::ICON, "folder"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Last Modified"}, {FMH::MODEL_KEY::VALUE, file.lastModified().toString()}, {FMH::MODEL_KEY::ICON, "view-media-recent"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Date"}, {FMH::MODEL_KEY::VALUE, file.birthTime().toString()}, {FMH::MODEL_KEY::ICON, "view-calendar-birthday"}};
  

    return res;
}




void PicInfoModel::parse()
{
    qDebug() << "PARSE @gadominguez URI: " << m_url.toString();
    if (!FMH::fileExists(m_url) || m_url.isEmpty() || !m_url.isValid()) {
        return;
    }
    
    QFileInfo file(m_url.toLocalFile());
    m_fileName = file.fileName();
    emit fileNameChanged();
    
    emit preListChanged();
    m_data.clear();
    m_data << basicInfo(m_url);
    FMH::MODEL model;
    
    
       Exiv2Extractor extractor(m_url);
       if (extractor.error())
       {
           return;
       }
       
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Aperture"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.MaxApertureValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Camera"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Image.Make")}, {FMH::MODEL_KEY::ICON, "camera-video"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Camera Model"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Image.Model")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Focal Length"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.FocalLength")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};

       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Exposure Time"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.ExposureTime")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};       
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Exposure Mode"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.ExposureMode")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Flash"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.Flash")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};

       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "ISO"}, {FMH::MODEL_KEY::VALUE, extractor.getExifTagString("Exif.Photo.ISOSpeedRatings")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Shutter Speed"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.ShutterSpeedValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Exposure"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.ExposureTime")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "White Balance"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Thumbnail.Compression")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Color Space"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.ColorSpace")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Compression"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Photo.WhiteBalance")}, {FMH::MODEL_KEY::ICON, "documentinfo"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Notes"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifComment()}, {FMH::MODEL_KEY::ICON, "note"}};
       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Author"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.Image.Artist")}, {FMH::MODEL_KEY::ICON, "user"}};

       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "GPS Latitude"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.GPSInfo.GPSLatitude")}, {FMH::MODEL_KEY::ICON, "user"}};

       m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "GPS Longitude"}, {FMH::MODEL_KEY::VALUE,  extractor.getExifTagString("Exif.GPSInfo.GPSLongitude")}, {FMH::MODEL_KEY::ICON, "user"}};

       
      qDebug() << "ISOOOOOOOOO" << extractor.getExifTagsDataList();
//        

    //    double latitude = extractor.gpsLatitude();
    //    double longitude = extractor.gpsLongitude();

    //    if (latitude != 0.0 && longitude != 0.0)
    //    {
    //        if (!m_geoCoder->initialized())
    //        {
    //            m_geoCoder->init();
    //        }
    //        QVariantMap map = m_geoCoder->lookup(latitude, longitude);

    //        QGeoAddress addr;
    //        addr.setCountry(map.value("country").toString());
    //        addr.setState(map.value("admin1").toString());
    //        addr.setCity(map.value("admin2").toString());
    //        m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Location"}, {FMH::MODEL_KEY::VALUE, addr.text()}};
    //    }
    //    m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Origin"}, {FMH::MODEL_KEY::VALUE, extractor.dateTime().toString()}};


   

//     Exiv2::ExifData::const_iterator end = exifData.end();
//     
//     for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
//         const char* tn = i->typeName();
//         QString val, key;
//         
//         for(auto v : i->key())
//         {
//             key.push_back(v);
//         }
//         
//         for(auto v : i->value().toString())
//         {
//             val.push_back(v) ;
//         }
//         
//         QString gpsLong;
//         for(auto g : gpsToString(exifData["Exif.GPSInfo.GPSLongitude"]))
//         {
//             gpsLong.push_back(g);
//         }
//         
//         if(key == "Exif.GPSInfo.GPSLongitude")
//         {
//             qDebug() << gpsLong;
//             qDebug() <<gpsLong.split(" ")[12].replace("'", "");;
//         }
//         
//         QString gpsLat;
//         for(auto g : gpsToString(exifData["Exif.GPSInfo.GPSLatitude"]))
//         {
//             gpsLat.push_back(g);
//         }
//         
//         if(key == "Exif.GPSInfo.GPSLongitude")
//         {
//             qDebug() << gpsLat;
//             qDebug() << gpsLat.split(" ")[12].replace("'", "");
//         }
// 
//         QString gps = gpsLat.split(" ")[12].replace("'", "") + ", " + gpsLong.split(" ")[12].replace("'", "");
//         m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Location"}, {FMH::MODEL_KEY::VALUE, gps}};
//         qDebug() << key + ": " + val;

        /*qDebug() << QString::number(std::setw(44)._M_n) << (char) std::setfill(' ')._M_c << std::left
                  << i->key()[0] << " "
                  << "0x" << std::setw(4)._M_n << std::setfill('0')._M_c << std::right
                  << std::hex << i->tag() << " "
                  << std::setw(9)._M_n << std::setfill(' ')._M_c << std::left
                  << (tn ? tn : "Unknown") << " "
                  << std::dec << std::setw(3)._M_n
                  << std::setfill(' ')._M_c << std::right
                  << i->count() << "  "
                  << i->value().toString()[0]
                  << "\n";*/
//     }

    emit postListChanged();
}

const FMH::MODEL_LIST &PicInfoModel::items() const
{
    return m_data;
}


std::string gpsToString(Exiv2::Metadatum& value)
{
    char r[500];
    int l = 0;
    l += sprintf(r+l,"count = %ld ",value.count());
    l += sprintf(r+l,"type = %d ",value.typeId());
    switch ( value.typeId() ) {
        case Exiv2::unsignedRational:
        {
            l += sprintf(r+l,"Exiv2::unsignedRational ");
            double decimal = 0 ;
            double denom   = 1 ;
            for ( int i = 0 ; i < value.count() ; i++) {
                Exiv2::Rational rational = value.toRational(i);
                l += sprintf(r+l,"%d/%d ",rational.first,rational.second);
                decimal += value.toFloat(i) / denom;
                denom   *= 60;
            }
            l+= sprintf(r+l,"decimal = '%f' ",decimal);
            l+= sprintf(r+l,"string = '%s' ",value.toString().c_str());
        } break;

        default:
            break;
    }

    return std::string(r);
}
