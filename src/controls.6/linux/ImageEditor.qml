import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

import org.kde.kquickimageeditor as KQuickImageEditor

import "../private" as Private


/**
 * @inherit org::mauikit::controls::Page
 * @brief A control with different tools for editingan image
 * 
 */
Maui.Page
{
    id: control
   
   property url url

    readonly property bool ready : String(control.url).length
    
    readonly property alias editor : imageDoc

    headBar.visible: control.ready

    headBar.leftContent: ToolButton
    {
        icon.name: "edit-undo"
        enabled: imageDoc.edited
        onClicked: imageDoc.undo()
    }

    headBar.middleContent: Maui.ToolActions
    {
        id: _editTools
        Layout.alignment: Qt.AlignHCenter
        autoExclusive: true
        property int currentIndex : 1
        expanded: control.width > Maui.Style.units.gridUnit * 30
        display: ToolButton.TextBesideIcon

        Action
        {
            text: i18nd("mauikitimagetools","Color")
            checked: _editTools.currentIndex === 0
            onTriggered: _editTools.currentIndex = 0
        }

        Action
        {
            text: i18nd("mauikitimagetools","Transform")
            checked: _editTools.currentIndex === 1
            onTriggered: _editTools.currentIndex = 1
        }

        Action
        {
            text: i18nd("mauikitimagetools","Layer")
            checked: _editTools.currentIndex === 2
            onTriggered: _editTools.currentIndex = 2
        }
    }

    KQuickImageEditor.ImageItem
    {
        id: editImage
        readonly property real ratioX: editImage.paintedWidth / editImage.nativeWidth;
        readonly property real ratioY: editImage.paintedHeight / editImage.nativeHeight;

        fillMode: KQuickImageEditor.ImageItem.PreserveAspectFit
        image: imageDoc.image
        anchors.fill: parent
        anchors.margins: Maui.Style.space.medium

        rotation: _transBar.rotationSlider.value

        KQuickImageEditor.ImageDocument
        {
            id: imageDoc
            path: control.url
        }

        KQuickImageEditor.SelectionTool
        {
            id: selectionTool
            visible: _transBar.cropButton.checked
            width: editImage.paintedWidth
            height: editImage.paintedHeight
            x: editImage.horizontalPadding
            y: editImage.verticalPadding

            KQuickImageEditor.CropBackground
            {
                anchors.fill: parent
                z: -1
                insideX: selectionTool.selectionX
                insideY: selectionTool.selectionY
                insideWidth: selectionTool.selectionWidth
                insideHeight: selectionTool.selectionHeight
            }
            Connections {
                target: selectionTool.selectionArea
                function onDoubleClicked() {
                    control.crop()
                }
            }
        }

        onImageChanged:
        {
            selectionTool.selectionX = 0
            selectionTool.selectionY = 0
            selectionTool.selectionWidth = Qt.binding(() => selectionTool.width)
            selectionTool.selectionHeight = Qt.binding(() => selectionTool.height)
        }


        Canvas
        {
            visible: _transBar.rotationButton.checked
            opacity: 0.15
            anchors.fill : parent
            property int wgrid: control.width / 20
            onPaint: {
                var ctx = getContext("2d")
                ctx.lineWidth = 0.5
                ctx.strokeStyle = Maui.Theme.textColor
                ctx.beginPath()
                var nrows = height/wgrid;
                for(var i=0; i < nrows+1; i++){
                    ctx.moveTo(0, wgrid*i);
                    ctx.lineTo(width, wgrid*i);
                }

                var ncols = width/wgrid
                for(var j=0; j < ncols+1; j++){
                    ctx.moveTo(wgrid*j, 0);
                    ctx.lineTo(wgrid*j, height);
                }
                ctx.closePath()
                ctx.stroke()
            }
        }
    }

    footBar.visible: false
    footerColumn: [

        Private.TransformationBar
        {
            id: _transBar
            visible: _editTools.currentIndex === 1 && control.ready
            width: parent.width
        },

        Private.ColourBar
        {
            id: _colourBar
            visible: _editTools.currentIndex === 0 && control.ready
            width: parent.width
        }
    ]


    function crop() {
        console.log("CROP")
        imageDoc.crop(selectionTool.selectionX / editImage.ratioX,
                      selectionTool.selectionY / editImage.ratioY,
                      selectionTool.selectionWidth / editImage.ratioX,
                      selectionTool.selectionHeight / editImage.ratioY);
    }
}
