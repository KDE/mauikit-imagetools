// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later


import QtQuick 2.13

import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.8 as Kirigami

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FB
import org.mauikit.imagetools 1.3 as IT

Maui.Dialog
{
    id: control
    property alias url : _infoModel.url
    maxHeight: 800
    maxWidth: 500
    hint: 1
    defaultButtons: false
    title: _infoModel.fileName
    headBar.visible: true
    spacing: 0

    Rectangle
    {
        Layout.fillWidth: true
        Layout.preferredHeight: 200
        color: Qt.darker(Kirigami.Theme.backgroundColor, 1.1)

        Image
        {
            id: _img
            anchors.fill: parent
            source: control.url
            fillMode: Image.PreserveAspectCrop

            Rectangle
            {
                color: "#333"
                opacity: 0.5
                anchors.fill: parent
            }

            Rectangle
            {
                anchors.centerIn: parent
                color: "#333"
                radius: Maui.Style.radiusV
                width: 100
                height: 32
                Label
                {
                    anchors.centerIn: parent
                    text: _img.sourceSize.width + " x " + _img.sourceSize.height
                    color: "white"
                }
            }
        }
    }

    FB.TagsBar
    {
        Layout.fillWidth: true
        visible: count > 0
        allowEditMode: false
        list.urls: [control.url]
        list.strict: false
    }

    Maui.Separator
    {
        edge: Qt.BottomEdge
        Layout.fillWidth: true
    }

    Repeater
    {
        model: Maui.BaseModel
        {
            list: IT.PicInfoModel
            {
                id:_infoModel
            }
        }

        Maui.AlternateListItem
        {
            visible: model.value.length
            Layout.fillWidth: true
            implicitHeight: visible ? _delegateColumnInfo.implicitHeight + Maui.Style.space.large : 0
            lastOne: index === _infoModel.count-1

            Maui.ListItemTemplate
            {
                id: _delegateColumnInfo
                width: parent.width

                iconSource: model.icon
                iconSizeHint: Maui.Style.iconSizes.medium
                spacing: Maui.Style.space.medium
                anchors.centerIn: parent
                anchors.margins: Maui.Style.space.medium

                label1.text: model.key
                label1.font.weight: Font.Bold
                label1.font.bold: true
                label2.text: model.value
                label2.font.weight: Font.Light
            }
        }
    }
}
