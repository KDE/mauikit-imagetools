// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "imagemetadata.h"

static QStringList standardExifKeys { "Exif.Photo.DateTimeOriginal"};

ImageMetadata::ImageMetadata(QObject *parent) : QObject(parent)
{
    
}
