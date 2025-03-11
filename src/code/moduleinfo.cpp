#include "moduleinfo.h"
#include "../imagetools_version.h"
#include <KLocalizedString>
#include <exiv2/version.hpp>
#include "ocs.h"

QString MauiKitImageTools::versionString()
{
    return QStringLiteral(ImageTools_VERSION_STRING);
}

QString MauiKitImageTools::buildVersion()
{
    return QStringLiteral(GIT_BRANCH)+QStringLiteral("/")+QStringLiteral(GIT_COMMIT_HASH);
}

KAboutComponent MauiKitImageTools::aboutData()
{
    return KAboutComponent(QStringLiteral("MauiKit ImageTools"),
                           i18n("Image tools components for editing, viewing and navigating."),
                           QStringLiteral(ImageTools_VERSION_STRING),
                           QStringLiteral("http://mauikit.org"),
                           KAboutLicense::LicenseKey::LGPL_V3);
}

KAboutComponent MauiKitImageTools::exiv2Data()
{
    return KAboutComponent(QStringLiteral("Exiv2"),
                           QStringLiteral("Exiv2 is a C++ library and a command-line utility to read, write, delete and modify Exif, IPTC, XMP and ICC image metadata."),
                           QString::fromStdString(Exiv2::versionString()),
                           QStringLiteral("https://exiv2.org"),
                           KAboutLicense::LicenseKey::File);
}

KAboutComponent MauiKitImageTools::tesseractData()
{
    return KAboutComponent(QStringLiteral("Tesseract"),
                           QStringLiteral("Open source OCR engine."),
                           OCS::versionString(),
                           QStringLiteral("https://github.com/tesseract-ocr/tesseract"),
                           KAboutLicense::LicenseKey::BSD_2_Clause);
}
