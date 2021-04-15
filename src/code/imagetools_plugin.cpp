// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QQmlEngine>
#include <QResource>

#include "imagetools_plugin.h"

void ImageToolsPlugin::registerTypes(const char *uri)
{
#if defined(Q_OS_ANDROID)
    QResource::registerResource(QStringLiteral("assets:/android_rcc_bundle.rcc"));
    #endif
    
    qmlRegisterType(resolveFileUrl(QStringLiteral("ImageViewer.qml")), uri, 1, 0, "ImageViewer");   
    qmlRegisterType(resolveFileUrl(QStringLiteral("ImageEditor.qml")), uri, 1, 0, "ImageEditor"); 
}

