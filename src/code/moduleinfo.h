#pragma once

#include <QString>
#include <KAboutData>
#include "imagetools_export.h"

/**
 * @brief The MauiKit module information
 */
namespace MauiKitImageTools
{
   /**
    * @brief
    */
   IMAGETOOLS_EXPORT QString versionString();
   
   /**
    * @brief
    */
   IMAGETOOLS_EXPORT QString buildVersion();
   
   /**
    * @brief
    */
   IMAGETOOLS_EXPORT KAboutComponent aboutData();
   
   /**
    * @brief
    */
   IMAGETOOLS_EXPORT KAboutComponent exiv2Data();
   IMAGETOOLS_EXPORT KAboutComponent libKexiv2Data();
   IMAGETOOLS_EXPORT KAboutComponent opencvData();

   /**
    * @brief
    */
   IMAGETOOLS_EXPORT KAboutComponent tesseractData();

};
