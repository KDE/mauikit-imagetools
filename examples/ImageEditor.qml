import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.mauikit.calendar as MC

import org.mauikit.imagetools as IT

Maui.ApplicationWindow
{
    id: root

    Maui.Page
    {
        anchors.fill: parent
        Maui.Controls.showCSD: true
        title: root.title

        IT.ImageEditor
        {
            anchors.fill: parent
            url: "file:///home/camiloh/maui-demo-files/Pictures/4416d027-9fa4-4762-8eb6-31de331623a1.jpg"
        }
    }
}

