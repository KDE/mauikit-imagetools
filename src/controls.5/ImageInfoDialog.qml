// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later


import QtQuick 2.13

import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import QtLocation 5.6
import QtPositioning 5.6

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FB
import org.mauikit.imagetools 1.3 as IT

Maui.PopupPage
{
    id: control

    property alias url : _infoModel.url

    maxHeight: 800
    maxWidth: 500
    hint: 1

    title: _infoModel.fileName
    headBar.visible: true
    spacing: Maui.Style.space.huge
    
    footBar.rightContent: [
        ToolButton
        {
            icon.name: "list-add"
            text: "Add Exif tag"
            onClicked: _editTagDialog.open()
        },
        
        ToolButton
        {
            icon.name: "file-open"
            text: "Open"
        }        
    ]

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
//            sourceSize.width: width
//            sourceSize.height: height

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
                    text: _img.implicitWidth + " x " + _img.implicitHeight
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
    
    Maui.InfoDialog
    {
        id: _editTagDialog
        property alias key : _keyField.text
        property alias value : _valueField.text
        
        title: i18n ("Edit")
        message: i18nd("mauikitimagetools","Editing Exif tag")
        
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

            delegate: Maui.SectionItem
            {
                visible: model.value && String(model.value).length > 0
                Layout.fillWidth: true
                label1.text: model.name
                label2.text: model.value
                columns: 3
                
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
    }

    Maui.Separator
    {
        Layout.fillWidth: true
        visible: map.visible
    }

    Map
    {
        id: map
        visible: _infoModel.lat !== 0 &&  _infoModel.lon !== 0
        color: Maui.Theme.backgroundColor
        Layout.fillWidth: true
        Layout.preferredHeight: 400
        gesture.acceptedGestures: MapGestureArea.NoGesture
        gesture.flickDeceleration: 3000
        gesture.enabled: true

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
