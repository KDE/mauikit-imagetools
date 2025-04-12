/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick

import org.mauikit.controls as Maui
import org.mauikit.imagetools.editor as ITE

ITE.ResizeHandle
{
    width: Kirigami.Settings.isMobile ? 20 : 10
    height: width

    Maui.ShadowedRectangle
    {
        Maui.Theme.colorSet: Maui.Theme.View
        color: Maui.Theme.backgroundColor
        shadow {
            size: 4
            color: Maui.Theme.textColor
        }
        anchors.fill: parent
        radius: width
        opacity: 0.8
    }
    scale: 1 
}
