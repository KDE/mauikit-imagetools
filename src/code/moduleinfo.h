#pragma once

#include <QString>
#include <KAboutData>
#include "imagetools_export.h"

namespace MauiKitImageTools
{
   IMAGETOOLS_EXPORT QString versionString();
   IMAGETOOLS_EXPORT QString buildVersion();
   IMAGETOOLS_EXPORT KAboutComponent aboutData();
   IMAGETOOLS_EXPORT KAboutComponent exiv2Data();

};
