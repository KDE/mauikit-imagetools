/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "command.h"

#include <QImage>
#include <QTransform>

/**
 * @brief RotateCommand that rotates the current image.
 */
class RotateCommand : public Command
{
public:
    /**
     * Contructor
     */
    RotateCommand(const QTransform &m_tranform);
    ~RotateCommand() override = default;

    QImage redo(QImage image) override;

    QImage undo(QImage image) override;

private:
    QTransform m_tranform;
    QImage m_image;
};
