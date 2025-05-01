// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later


import QtQuick

import QtQuick.Layouts
import QtQuick.Controls 

import QtLocation
import QtPositioning 

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB
import org.mauikit.imagetools as IT

/**
 * @inherit org::mauikit::controls::PopupPage
 * @brief A popup view presenting with metadata information about a given image file.
 *
 * @image html imageinfodialog.png "Image information dialog"
 *
 * @code
 * IT.ImageInfoDialog
 * {
 *  id: _dialog
 *  url: "file:///home/camiloh/maui-demo-files/Pictures/4416d027-9fa4-4762-8eb6-31de331623a1.jpg"
 * }
 * @endcode
 */
Maui.PopupPage
{
    id: control

    /**
     * @brief
     */
    property alias url : _infoModel.url

    maxHeight: 800
    maxWidth: 500
    hint: 1

    title: _infoModel.fileName
    headBar.visible: true
    spacing: Maui.Style.space.huge

    signal gpsEdited(var url)
    
    Rectangle
    {
        Layout.fillWidth: true
        Layout.preferredHeight: 200
        color: Qt.darker(Maui.Theme.backgroundColor, 1.1)

        Image
        {
            id: _img
            anchors.fill: parent
            source: control.url
            fillMode: Image.PreserveAspectCrop
            sourceSize.width: width
            sourceSize.height: height

            Rectangle
            {
                color: "#333"
                opacity: 0.5
                anchors.fill: parent
            }

            Label
            {
                anchors.centerIn: parent
                text: _infoModel.pixelSize.width + " x " +  _infoModel.pixelSize.height
                color: "white"
                padding: Maui.Style.defaultPadding
                background: Rectangle
                {
                    color: "#333"
                    radius: Maui.Style.radiusV
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

    Maui.InfoDialog
    {
        id: _editTagDialog
        property alias key : _keyField.text
        property alias value : _valueField.text

        title: i18n ("Edit")
        message: i18nd("mauikitimagetools","Editing Exif tag")

        standardButtons: Dialog.Save | Dialog.Cancel

        TextField
        {
            id: _keyField
            Layout.fillWidth: true
            placeholderText: i18nd("mauikitimagetools","Tag key")
        }

        TextField
        {
            id: _valueField
            Layout.fillWidth: true
            placeholderText: i18nd("mauikitimagetools","Tag value")
        }

        onAccepted:
        {
            console.log(_editTagDialog.key, _editTagDialog.value)
            if(_infoModel.editTag(_editTagDialog.key, _editTagDialog.value))
            {
                _editTagDialog.close()
            }else
            {
                _editTagDialog.alert(i18nd("mauikitimagetools","Could not edit the tag"), 2)
            }
        }

        onRejected:
        {
            _editTagDialog.close()
        }

        function set(key, value)
        {
            _editTagDialog.key = key
            _editTagDialog.value = value
            _editTagDialog.open()
        }
    }

    Maui.InfoDialog
    {
        id: _removeTagDialog
        property string key
        property string value

        title: i18n ("Remove")
        message: i18nd("mauikitimagetools","Are you sure you want to remove the Exif tag %1?", _removeTagDialog.value)

        standardButtons: Dialog.Yes | Dialog.Cancel

        onAccepted:
        {
            if(_infoModel.removeTag(_removeTagDialog.key))
            {
                _removeTagDialog.close()
            }else
            {
                _removeTagDialog.alert(i18nd("mauikitimagetools","Could not remove the tag"), 2)
            }
        }

        onRejected:
        {
            _removeTagDialog.close()
        }

        function set(key, value)
        {
            _removeTagDialog.key = key
            _removeTagDialog.value = value
            _removeTagDialog.open()
        }
    }

    Maui.InfoDialog
    {
        id: _commentDialog

        title: i18n ("Comment")

        TextArea
        {
            id: _commentArea
            Layout.fillWidth: true
            Layout.preferredHeight: 200

            text: _infoModel.exifComment
        }

        standardButtons: Dialog.Save | Dialog.Cancel
        onAccepted:
        {
            _infoModel.setComment(_commentArea.text)
            _commentDialog.close()
        }

        onRejected: _commentDialog.close()
    }

    Maui.InfoDialog
    {
        id: _gpsTagDialog

        title: i18n ("GPS")

        standardButtons: Dialog.Save | Dialog.Cancel

        Maui.TextField
        {
            id: _lat
            text: _infoModel.lat
            Maui.Controls.title: placeholderText
            placeholderText: i18n("Latitude")
            validator: DoubleValidator
            {
                notation: DoubleValidator.StandardNotation
            }
            Layout.fillWidth: true
        }

        Maui.TextField
        {
            id: _lon
            placeholderText: i18n("Longitude")
            Maui.Controls.title: placeholderText

            text: _infoModel.lon
            validator: DoubleValidator
            {
                notation: DoubleValidator.StandardNotation
            }
            Layout.fillWidth: true
        }

        // Maui.TextField
        // {
        //     id: _alt
        //     placeholderText: i18n("Altitude")
        //     Maui.Controls.title: placeholderText

        //     text: _infoModel.alt
        //     validator: DoubleValidator
        //     {
        //         notation: DoubleValidator.StandardNotation
        //     }

        //     // onAcceptableInputChanged: color = acceptableInput ? "black" : "red";

        //     Layout.fillWidth: true
        // }

        onAccepted:
        {
            if(_infoModel.setGpsData(_lat.text, _lon.text))
            {
                control.gpsEdited(control.url)
                _editTagDialog.close()
            }else
            {
                _editTagDialog.alert(i18nd("mauikitimagetools","Could not save the GPS data"), 2)
            }
        }

        onRejected:
        {
            _gpsTagDialog.close()
        }
    }

    Maui.SectionGroup
    {
        Layout.fillWidth: true

        title: i18nd("mauikitimagetools","Details")
        description: i18nd("mauikitimagetools","File information")

        Repeater
        {
            model: Maui.BaseModel
            {
                list: IT.PicInfoModel
                {
                    id:_infoModel
                }
            }

            delegate: Maui.FlexSectionItem
            {
                visible: model.value && String(model.value).length > 0
                label1.text: model.name
                label2.text: model.value

                ToolButton
                {
                    visible: model.key
                    icon.name: "document-edit"
                    onClicked: _editTagDialog.set(model.key, model.value)
                }

                ToolButton
                {
                    visible: model.key
                    icon.name: "edit-delete"
                    onClicked: _removeTagDialog.set(model.key, model.value)
                }
            }
        }

        Button
        {
            Layout.fillWidth: true

            // icon.name: "list-add"
            text: "Add Exif tag"
            onClicked: _editTagDialog.open()
        }
    }

    Maui.SectionGroup
    {
        Layout.fillWidth: true

        title: i18nd("mauikitimagetools","Comment & Description")

        TextArea
        {
            text: _infoModel.exifComment
            visible: text.length > 0
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            readOnly: true
        }

        RowLayout
        {
            spacing: Maui.Style.defaultSpacing
            Layout.fillWidth: true

            Button
            {
                text: i18n("Add Comment")
                Layout.fillWidth: true
                onClicked: _commentDialog.open()
            }

            Button
            {
                text: i18n("Remove Comment")
                Layout.fillWidth: true
                visible: _infoModel.exifComment.length>0
                onClicked: _infoModel.removeComment()
                Maui.Controls.status: Maui.Controls.Negative
            }
        }
    }

    Maui.SectionGroup
    {
        Layout.fillWidth: true

        title: i18nd("mauikitimagetools","GPS")
        description: i18nd("mauikitimagetools","Geolocation tags")

        Maui.FlexSectionItem
        {
            visible: _mapLoader.visible
            label1.text: i18n("City")
            label2.text: _infoModel.cityName
        }

        RowLayout
        {
            spacing: Maui.Style.defaultSpacing
            Layout.fillWidth: true

            Button
            {
                Layout.fillWidth: true
                text: i18n("Set GPS info")
                onClicked: _gpsTagDialog.open()
            }

            Loader
            {
                asynchronous: true
                active: _mapLoader.active
                visible: active
                Layout.fillWidth: true

                sourceComponent: Button
                {
                    text: i18n("Remove GPS info")
                    Maui.Controls.status: Maui.Controls.Negative
                    onClicked: _infoModel.removeGpsData()
                }
            }
        }

        Loader
        {
            id: _mapLoader
            asynchronous: true
            active: visible
            visible: _infoModel.lat !== 0 &&  _infoModel.lon !== 0
            Layout.fillWidth: true
            Layout.preferredHeight: 400

            sourceComponent: Map
            {
                id: map
                color: Maui.Theme.backgroundColor

                // gesture.acceptedGestures: MapGestureArea.NoGesture
                // gesture.flickDeceleration: 3000
                // gesture.enabled: true

                plugin: Plugin
                {
                    id: mapPlugin
                    name: "osm" // "mapboxgl", "esri", ...
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

                MapCircle
                {
                    center: map.center
                    radius: 50.0
                    color: Maui.Theme.highlightColor
                }

                Component.onCompleted:
                {
                    map.addMapItem(map.circle)
                }
            }
        }
    }
}
