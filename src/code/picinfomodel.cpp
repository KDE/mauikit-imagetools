// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "picinfomodel.h"
#include "exiv2extractor.h"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QUrl>

PicInfoModel::PicInfoModel(QObject *parent)  : MauiList(parent)
    , m_extractor(new Exiv2Extractor(this))

{
}

void PicInfoModel::componentComplete()
{
    connect(this, &PicInfoModel::urlChanged, [this](QUrl)
            {
                if (!FMH::fileExists(m_url) || m_url.isEmpty() || !m_url.isValid()) {
                    return;
                }

                QFileInfo file(m_url.toLocalFile());
                m_fileName = file.fileName();
                Q_EMIT fileNameChanged();

                m_extractor->setUrl(m_url);
                this->parse();
            });
    
    if (FMH::fileExists(m_url) && !m_url.isEmpty() && m_url.isValid()) {
        m_extractor->setUrl(m_url);
        this->parse();
    }    
}

void PicInfoModel::setUrl(QUrl url)
{
    if (m_url == url)
    {
        return;
    }
    
    m_url = url;
    Q_EMIT urlChanged(m_url);
}

bool PicInfoModel::removeTag(const QString& tag)
{
    if(!m_extractor)
        return false;
    
    if(m_extractor->removeTag(tag.toStdString().c_str()))
    {        
        this->parse();
        return true;
    }
    
    return false;
}

bool PicInfoModel::editTag(const QString& tag, const QString& value)
{   
    qDebug() << "trying to write tag1";

    if(m_extractor->writeTag(tag.toStdString().c_str(), QVariant::fromValue(value)))
    {        
        qDebug() << "trying to write tag3";
        if(m_extractor->applyChanges())
        {
            this->parse();
            return true;
        }else
        {
            qWarning() << "failed to apply tag changes";
        }
    }else
    {
        qWarning() << "failed to write the metadata value tag" << tag << value;
    }
    
    return false;
}

bool PicInfoModel::setGpsData( const QString &latitude, const QString &longitude, const QString &altitude)
{
    bool ok = false;
    if( m_extractor->setGpsData(latitude.toDouble(&ok), longitude.toDouble(&ok), altitude.toDouble(&ok)))
    {
        qDebug() << "gps data has been set" << ok;
        if(m_extractor->applyChanges())
        {
            this->parse();
            return true;
        }else
        {
            qWarning() << "failed to apply tag changes";
        }
    }else
    {
        qWarning() << "failed to set gps data to image";
    }
    return false;

}

bool PicInfoModel::setComment(const QString &comment)
{
    return m_extractor->setComment(comment);
}

double PicInfoModel::latitude() const
{
    return m_latitude;
}

double PicInfoModel::longitude() const
{
    return m_longitude;
}

double PicInfoModel::altitude() const
{
    return m_altitude;
}

QUrl PicInfoModel::url() const
{
    return m_url;
}

static FMH::MODEL_LIST basicInfo(const QUrl &url)
{
    FMH::MODEL_LIST res;
    QFileInfo file(url.toLocalFile());
    QLocale locale;
    res << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Name"}, {FMH::MODEL_KEY::VALUE, file.fileName()}, {FMH::MODEL_KEY::ICON, "edit-rename"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Path"}, {FMH::MODEL_KEY::VALUE, url.toLocalFile()}, {FMH::MODEL_KEY::ICON, "folder"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Size"}, {FMH::MODEL_KEY::VALUE, locale.formattedDataSize(file.size())}, {FMH::MODEL_KEY::ICON, "folder"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Last Modified"}, {FMH::MODEL_KEY::VALUE, file.lastModified().toString()}, {FMH::MODEL_KEY::ICON, "view-media-recent"}};
    res << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Date"}, {FMH::MODEL_KEY::VALUE, file.birthTime().toString()}, {FMH::MODEL_KEY::ICON, "view-calendar-birthday"}};
    
    
    return res;
}

void PicInfoModel::parse()
{
    qDebug() << "Setting image medatata model info";
    Q_EMIT preListChanged();
    m_data.clear();
    m_data << basicInfo(m_url);

    if (!m_extractor->error())
    {
        auto gps = m_extractor->extractGPS();
        m_latitude = gps.latitude;
        m_longitude = gps.longitude;
        m_altitude = gps.altitude;

        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Max Aperture"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.MaxApertureValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.MaxApertureValue"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Aperture"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.ApertureValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Image.ApertureValue"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "BrightnessValue"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.BrightnessValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Image.BrightnessValue"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Camera"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Make")}, {FMH::MODEL_KEY::ICON, "camera-video"}, {FMH::MODEL_KEY::KEY, "Exif.Image.Make"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Camera Model"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Model")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Image.Model"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Focal Length"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.FocalLength")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.FocalLength"}};        
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Exposure Time"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.ExposureTime")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.ExposureTime"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Exposure Mode"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.ExposureMode")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.ExposureMode"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Flash"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.Flash")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.Flash"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "ISO Speed"}, {FMH::MODEL_KEY::VALUE, m_extractor->getExifTagString("Exif.Photo.ISOSpeedRatings")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.ISOSpeedRatings"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Shutter Speed"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.ShutterSpeedValue")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.ShutterSpeedValue"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Compression"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Thumbnail.Compression")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Thumbnail.Compression"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Color Space"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.ColorSpace")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.ColorSpace"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "White Balance"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Photo.WhiteBalance")}, {FMH::MODEL_KEY::ICON, "documentinfo"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.WhiteBalance"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Notes"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifComment()}, {FMH::MODEL_KEY::ICON, "note"}, {FMH::MODEL_KEY::KEY, "Exif.Photo.UserComment"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Author"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Artist")}, {FMH::MODEL_KEY::ICON, "user"}, {FMH::MODEL_KEY::KEY, "Exif.Image.Artist"}};
        
        // if(m_latitude != 0)
        //     m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "GPS Latitude"}, {FMH::MODEL_KEY::VALUE, QString::number(m_latitude)}, {FMH::MODEL_KEY::ICON, "user"}, {FMH::MODEL_KEY::KEY, "Exif.GPSInfo.GPSLatitude"}};
        
        // if(m_longitude != 0)
        //     m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "GPS Longitude"}, {FMH::MODEL_KEY::VALUE,  QString::number(m_longitude)}, {FMH::MODEL_KEY::ICON, "user"}, {FMH::MODEL_KEY::KEY, "Exif.GPSInfo.GPSLongitude"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "City"}, {FMH::MODEL_KEY::VALUE,  m_extractor->GPSString ()}, {FMH::MODEL_KEY::ICON, "user"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Processing Software"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.ProcessingSoftware")}, {FMH::MODEL_KEY::KEY, "Exif.Image.ProcessingSoftware"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "DocumentName"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.DocumentName")}, {FMH::MODEL_KEY::KEY, "Exif.Image.DocumentName"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "ImageDescription"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.ImageDescription")}, {FMH::MODEL_KEY::KEY, "Exif.Image.ImageDescription"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Software"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Software")}, {FMH::MODEL_KEY::KEY, "Exif.Image.Software"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "HostComputer"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.HostComputer")}, {FMH::MODEL_KEY::KEY, "Exif.Image.HostComputer"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Rating"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Rating")}, {FMH::MODEL_KEY::KEY, "Exif.Image.Rating"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "Copyright"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.Copyright")}, {FMH::MODEL_KEY::KEY, "Exif.Image.Copyright"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "SelfTimerMode"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.SelfTimerMode")}, {FMH::MODEL_KEY::KEY, "Exif.Image.SelfTimerMode"}};
        
        m_data << FMH::MODEL{{FMH::MODEL_KEY::NAME, "ImageHistory"}, {FMH::MODEL_KEY::VALUE,  m_extractor->getExifTagString("Exif.Image.ImageHistory")}, {FMH::MODEL_KEY::KEY, "Exif.Image.ImageHistory"}};
        
        
    }else
    {
        qDebug() << "error extracting image metadata";
    }
    
    Q_EMIT postListChanged();
    Q_EMIT dataReady();
}

bool PicInfoModel::removeGpsData()
{
    if(m_extractor->removeGpsData())
    {
        if(m_extractor->applyChanges())
        {
            this->parse();
            return true;
        }
    }

    return false;
}

const FMH::MODEL_LIST &PicInfoModel::items() const
{
    return m_data;
}

