import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

ColumnLayout
{
    id: control

    spacing: 0

    property alias brightnessButton: _brightnessButton
    property alias contrastButton : _contrastButton
    property alias saturationButton : _saturationButton
    property Operation currentOperation : _saturationButton
    component Operation : ToolButton
    {
        id: _comp
        property int value
        property double stepSize
        property double from
        property double to
        display: ToolButton.TextOnly
        onValueChanged: slider.value = value

        property Slider slider: Slider
        {
            Layout.fillWidth: true

            Binding on value
            {
                value: _comp.value
                restoreMode: Binding.RestoreBindingOrValue
            }
            onMoved: _comp.value = value
            onValueChanged: _comp.value = value
            stepSize: _comp.stepSize
            from: _comp.from
            to: _comp.to


        }
    }

    Maui.ToolBar
    {
        id: _sliderToolBar
        Layout.fillWidth: true
        middleContent: currentOperation.slider
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

        middleContent: Row
        {
            Layout.alignment: Qt.AlignHCenter
            spacing: Maui.Style.defaultSpacing

            Operation
            {
                id: _saturationButton
                checkable: true
                checked:  currentOperation == this
                autoExclusive: true
                icon.name:  "transform-crop"
                text:  i18nc("@action:button Crop an image", "Saturation");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }

                // value: editor.saturation

                Binding on value
                {
                    value: editor.saturation
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: -255
                to: 255
                stepSize: 1
                onValueChanged:
                {
                    console.log("Adjust staturation", value)
                    editor.adjustSaturation(value)
                }
            }

            Operation
            {
                id: _contrastButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Contrast");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }                Binding on value
                {
                    value: editor.contrast
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: -255
                to: 255
                stepSize: 1
                onValueChanged:
                {
                    console.log("Adjust contrast", value)
                    editor.adjustContrast(value)
                }
            }

            Operation
            {
                id: _exposureButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Exposure");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
            }

            Operation
            {
                id: _highlightsButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Highlights");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
            }

            Operation
            {
                id: _shadowsButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Shadows");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
            }

            Operation
            {
                id: _brightnessButton
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Rotate an image", "Brightness");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
            }
        }



        rightContent:  ToolButton
        {
            //                    text: i18nd("mauikitimagetools","Cancel")
            icon.name: "dialog-cancel"
            onClicked: imageDoc.cancel()
        }
    }
}

