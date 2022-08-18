// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QObject>
/**
 * @todo write docs
 */
class ImageMetadata : public QObject
{
    Q_OBJECT
    
public:
    explicit ImageMetadata(QObject *parent = nullptr);
};

