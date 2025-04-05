/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "command.h"

Command::~Command()
{
}

void Command::addProperty(const QString &key, const QVariant &value)
{
    m_properties.insert(key, value);
}

bool Command::hasProperty(const QString &key)
{
    return m_properties.contains(key);
}

QVariant Command::getPropertyValue(const QString &key)
{
    return m_properties.value(key);
}
