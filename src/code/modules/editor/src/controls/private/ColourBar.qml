import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

ColumnLayout
{
    id: control

    spacing: 0

    property Operation currentOperation : _brightnessButton

    component Operation : ToolButton
    {
        id: _comp
        property int value
        property double stepSize : 10
        property double from
        property double to
        display: ToolButton.TextOnly
        onValueChanged: slider.value = value

        property Slider slider: Ruler
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
                id: _brightnessButton
                autoExclusive: true
                checked: currentOperation == this
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image brightness", "Brightness");

                Binding on value
                {
                    value: editor.brightness
                    restoreMode: Binding.RestoreBindingOrValue
                }

                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }

                from: -255
                to: 255
                onValueChanged:
                {
                    console.log("Adjust staturation", value)
                    editor.adjustBrightness(value)
                }
            }

            Operation
            {
                checkable: true
                checked: currentOperation == this
                autoExclusive: true
                icon.name:  "transform-crop"
                text:  i18nc("@action:button Change image saturation", "Saturation");
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
                onValueChanged:
                {
                    console.log("Adjust staturation", value)
                    editor.adjustSaturation(value)
                }
            }

            Operation
            {
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image contrast", "Contrast");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
                Binding on value
                {
                    value: editor.contrast
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: -255
                to: 255
                onValueChanged:
                {
                    console.log("Adjust contrast", value)
                    editor.adjustContrast(value)
                }
            }

            Operation
            {
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image hue", "Hue");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }
                Binding on value
                {
                    value: editor.hue
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: 0
                to: 180
                onValueChanged:
                {
                    console.log("Adjust hue", value)
                    editor.adjustHue(value)
                }
            }

            Operation
            {
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image sharpness", "Sharpness");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }

                Binding on value
                {
                    value: editor.sharpness
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: 0
                to: 100
                onValueChanged:
                {
                    editor.adjustSharpness(value)
                }
            }

            Operation
            {
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image gamma", "Gamma");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }

                Binding on value
                {
                    value: editor.gamma
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: -100
                to: 100
                onValueChanged:
                {
                    editor.adjustGamma(value)
                }
            }

            Operation
            {
                autoExclusive: true
                icon.name: "transform-rotate"
                checkable: true
                text: i18nc("@action:button Change image threshold", "Threshold");
                onClicked:
                {
                    currentOperation = this
                    editor.applyChanges()
                }

                Binding on value
                {
                    value: editor.threshold
                    restoreMode: Binding.RestoreBindingOrValue
                }

                from: 0
                to: 255
                onValueChanged:
                {
                    editor.adjustThreshold(value)
                }
            }
        }
    }
}

