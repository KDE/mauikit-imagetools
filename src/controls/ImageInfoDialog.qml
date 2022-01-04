// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later


import QtQuick 2.13

import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import QtLocation 5.6
import QtPositioning 5.6

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

    Maui.SettingsSection
    {
        Layout.fillWidth: true

        title: i18n("Details")
        description: i18n("File information")

        Repeater
        {
            model: Maui.BaseModel
            {
                list: IT.PicInfoModel
                {
                    id:_infoModel
                }
            }

            delegate: Maui.SettingTemplate
            {
                visible: model.value && String(model.value).length > 0
                Layout.fillWidth: true
                label1.text: model.key
                label2.text: model.value
            }
        }
    }

    Kirigami.Separator
    {
        Layout.fillWidth: true
//        weight: Kirigami.Separator.Th
        visible: map.visible
    }

    Map
    {
        id: map
        visible: _infoModel.lat !== 0 &&  _infoModel.lon !== 0
        color: Kirigami.Theme.backgroundColor
        Layout.fillWidth: true
        Layout.preferredHeight: 400
        implicitHeight: 400
        gesture.acceptedGestures: MapGestureArea.NoGesture
        gesture.flickDeceleration: 3000
        gesture.enabled: true

        plugin: Plugin
        {
            id: mapPlugin
            name: "mapboxgl" // "mapboxgl", "esri", ...
            // specify plugin parameters if necessary
            // PluginParameter {
            //     name:
            //     value:
            // }
        }
//        center: QtPositioning.coordinate(_infoModel.lat, _infoModel.lon) // Oslo
        zoomLevel: 16
        center
        {
            latitude: _infoModel.lat
            longitude:_infoModel.lon
        }
    }
}
