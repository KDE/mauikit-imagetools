import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// import QtQuick.Extras
// import QtQuick.Controls.Styles 

import org.mauikit.controls as Maui

Slider
{
    id: control

    live: false

    leftPadding: 0
    rightPadding:  0

    implicitHeight: Maui.Style.toolBarHeight
    // implicitWidth: width

    background: Item
    {
        id: _bg
        // Rectangle
        // {
        // anchors.fill: parent
        // color: "pink"
        // }


        Loader
        {
            // anchors.fill: parent
            asynchronous: true
            sourceComponent: Row
            {
                spacing: 5
                x: (control.horizontal ? control.visualPosition * (control.availableWidth - implicitWidth) : 0)
                y: _bg.height/2 - implicitHeight/2

                Repeater
                {
                    // model: Math.abs(control.to) - Math.abs(control.from)
                    model: Math.abs(control.width/5)
                    Rectangle
                    {
                        color: Maui.Theme.textColor
                        height: 16
                        width: 1
                        opacity:  index%10 === 0 ? 1: 0.5
                        // Text
                        // {
                        //     property int correctIndex : control.from - index

                        //     text: correctIndex
                        //         visible: index%10 === 0
                        //     color: "yellow"
                        // }
                    }

                }
            }
        }
    }

    handle: ColumnLayout
    {
        id: handle
        x: (control.horizontal ? control.visualPosition * (control.availableWidth - width) : 0)
        y:  0
        spacing: 0
        width: 32
        height: control.height
        //        implicitHeight: Maui.Style.iconSizes.medium

        Label
        {
            // font.bold: true
            // font.weight: Font.Bold
            // anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            // visible: control.pressed
            font.pointSize: Maui.Style.fontSizes.tiny

            // text: control.value
            text: control.pressed ?  control.valueAt(control.visualPosition ) : control.value
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            // background: Rectangle
            // {
            //     color: Maui.Theme.backgroundColor
            //     // radius: width/2
            // }
        }

        Maui.Icon
        {
            Layout.alignment: Qt.AlignBottom| Qt.AlignHCenter
            height: 32
            width: height
            color: Maui.Theme.textColor
            isMask: true
            source: "qrc:/assets/arrow-up.svg"
        }

    }
}
