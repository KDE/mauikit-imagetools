import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.kde.kquickimageeditor as KQuickImageEditor

ColumnLayout
{
    id: control

    spacing: 0

    property alias brightnessButton: _brightnessButton
    property alias contrastButton : _contrastButton
    property alias saturationButton : _saturationButton


    Maui.ToolBar
    {
        id: _sliderToolBar
        Layout.fillWidth: true
        middleContent:  Slider
        {
            id: _slider
            Layout.fillWidth: true
            value: 0
            from: -100
            to: 100
            stepSize: 1
        }

        background: Rectangle
        {
            color: Maui.Theme.backgroundColor
        }
    }

    Maui.ToolBar
    {
        position: ToolBar.Footer
        Layout.fillWidth: true

        background: Rectangle
        {
            color: Maui.Theme.backgroundColor
        }

        middleContent: [
            ToolButton
            {
                id: _saturationButton
                checkable: true
                autoExclusive: true
                icon.name:  "transform-crop"
                text:  i18nc("@action:button Crop an image", "Saturation");
            },

            ToolButton
            {
                id: _contrastButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Contrast");
            }  ,

            ToolButton
            {
                id: _exposureButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Exposure");
            },

            ToolButton
            {
                id: _highlightsButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Highlights");
            },

            ToolButton
            {
                id: _shadowsButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Shadows");
            },


            ToolButton
            {
                id: _brightnessButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Brightness");
            }
        ]

        leftContent: ToolButton
        {
            //                    text: i18nd("mauikitimagetools","Accept")
            icon.name: "dialog-apply"
            onClicked:
            {

            }
        }

        rightContent:  ToolButton
        {
            //                    text: i18nd("mauikitimagetools","Cancel")
            icon.name: "dialog-cancel"
            onClicked:
            {
            }
        }
    }
}

