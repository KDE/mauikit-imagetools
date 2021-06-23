// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "picinfomodel.h"
#include "exiv2extractor.h"

#include <QDateTime>
#include <QDebug>

std::string gpsToString(Exiv2::Metadatum& value);
PicInfoModel::PicInfoModel(QObject *parent)
: MauiList(parent)
{
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
    if (!FMH::fileExists(m_url) || m_url.isEmpty() || !m_url.isValid()) {
        return;
    }

    QFileInfo file(m_url.toLocalFile());
    m_fileName = file.fileName();
    emit fileNameChanged();

    emit preListChanged();
    m_data.clear();
    m_data << basicInfo(m_url);

    Exiv2Extractor extractor(m_url);
    if (!extractor.error())
    {
        m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "Location"}, {FMH::MODEL_KEY::VALUE,  extractor.GPSString()}, {FMH::MODEL_KEY::ICON, "map-globe"}};

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
        m_data << FMH::MODEL{{FMH::MODEL_KEY::KEY, "City"}, {FMH::MODEL_KEY::VALUE,  extractor.GPSString ()}, {FMH::MODEL_KEY::ICON, "user"}};

    }

    emit postListChanged();
}

const FMH::MODEL_LIST &PicInfoModel::items() const
{
    return m_data;
}

