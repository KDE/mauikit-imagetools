/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "rotatecommand.h"

RotateCommand::RotateCommand(const QTransform &tranform)
    : m_tranform(tranform)
{
}

QImage RotateCommand::undo(QImage image)
{
    return m_image;
}

QImage RotateCommand::redo(QImage image)
{
    m_image = image;
    return image.transformed(m_tranform);
}
