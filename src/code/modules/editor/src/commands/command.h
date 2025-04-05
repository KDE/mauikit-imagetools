/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "qimage.h"
#include <QObject>
#include <QMap>
#include <QVariant>

class QImage;

/**
 * A class implementing the command pattern. This is used to implemented various filters.
 */
class Command
{
public:
    virtual ~Command() = 0;

    /**
     * Applies the change to the document.
     */
    virtual QImage redo(QImage image) = 0;

    /**
     * Revert a change to the document.
     */
    virtual QImage undo(QImage image = {}) = 0;

    void addProperty(const QString &key, const QVariant &value );
    bool hasProperty(const QString &key);
    QVariant getPropertyValue(const QString &key);

private:
    QMap<QString, QVariant> m_properties;
};
