import QtQuick 
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import org.mauikit.controls as Maui
import org.mauikit.imagetools.editor as ITE

import "private" as Private


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

    signal saved()
    signal savedAs(string url)
    signal canceled()

    Maui.InfoDialog
    {
        id: _cancelDialog
        message: i18n("Are you sure you wanna cancel all the edits?")
        standardButtons: Dialog.Yes | Dialog.Cancel

        onAccepted:
        {
            imageDoc.cancel()
            control.canceled()
        }

        onRejected: close()
    }

    headBar.visible: control.ready
    headBar.background: null
    headBar.leftContent: Button
    {
        icon.name: "go-previous"
        Maui.Controls.status : imageDoc.edited ? Maui.Controls.Negative : Maui.Controls.Normal
        text: i18n("Cancel")
        onClicked:
        {
            if(imageDoc.edited)
                _cancelDialog.open()
            else
                control.canceled()
        }
    }

    headBar.rightContent: [

        ToolButton
        {
            icon.name: "document-save-as"
            enabled: imageDoc.edited
            onClicked:
            {
                imageDoc.saveAs()
                control.savedAs()
            }
        },

        Button
        {
            text: "Save"
            enabled: imageDoc.edited
            Maui.Controls.status : imageDoc.edited ? Maui.Controls.Positive : Maui.Controls.Normal

            onClicked:
            {
                imageDoc.save()
                control.saved()
            }
        }
    ]

    ITE.ImageItem
    {
        id: editImage
        readonly property real ratioX: editImage.paintedWidth / editImage.nativeWidth;
        readonly property real ratioY: editImage.paintedHeight / editImage.nativeHeight;

        fillMode: Image.PreserveAspectFit
        image: imageDoc.image
        anchors.fill: parent

        rotation: _transBar.rotationSlider.value

        ITE.ImageDocument
        {
            id: imageDoc
            path: control.url
        }

        ITE.SelectionTool
        {
            id: selectionTool
            width: editImage.paintedWidth
            height: editImage.paintedHeight
            x: editImage.horizontalPadding
            y: editImage.verticalPadding
            ITE.CropBackground
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
    }

    Canvas
    {
        visible: _transfromAction.checked
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

    Action
    {
        id: _colorsAction
        icon.name: "color-mode-black-white"
        text: i18nd("mauikitimagetools","Color")
        checked: _actionsBarLoader.currentIndex === 0
        onTriggered: _actionsBarLoader.currentIndex = 0
    }

    Action
    {
        id: _transfromAction
        icon.name: "dialog-transform"
        text: i18nd("mauikitimagetools","Transform")
        checked: _actionsBarLoader.currentIndex === 1
        onTriggered: _actionsBarLoader.currentIndex = 1
    }

    Action
    {
        id: _layerAction
        icon.name: "layer-new"
        text: i18nd("mauikitimagetools","Layer")
        checked: _actionsBarLoader.currentIndex === 3
        onTriggered: _actionsBarLoader.currentIndex = 3
    }

    Action
    {
        id: _filterAction
        icon.name: "image-auto-adjust"
        text: i18nd("mauikitimagetools","Filters")
        checked: _actionsBarLoader.currentIndex === 2
        onTriggered: _actionsBarLoader.currentIndex = 2
    }

    Loader
    {
        id: _actionsBarLoader
        property int currentIndex : 1
        // active: settings.showActionsBar
        visible: status == Loader.Ready
        asynchronous: true
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: Maui.Style.space.big

        sourceComponent: Pane
        {
            id: _pane
            Maui.Theme.colorSet: Maui.Theme.Complementary
            Maui.Theme.inherit: false

            // x: control.width - width - Maui.Style.space.big
            // y: control.height - height - Maui.Style.space.big
            background: Rectangle
            {
                radius: Maui.Style.radiusV
                color: Maui.Theme.backgroundColor

                layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
                layer.effect: MultiEffect
                {
                    autoPaddingEnabled: true
                    shadowEnabled: true
                    shadowColor: "#000000"
                }
            }

            ScaleAnimator on scale
            {
                from: 0
                to: 1
                duration: Maui.Style.units.longDuration
                running: visible
                easing.type: Easing.OutInQuad
            }

            OpacityAnimator on opacity
            {
                from: 0
                to: 1
                duration: Maui.Style.units.longDuration
                running: visible
            }

            contentItem:  RowLayout
            {
                spacing: Maui.Style.defaultSpacing

                ToolButton
                {
                    icon.name: "edit-undo"
                    Layout.alignment: Qt.AlignVCenter
                    onClicked: imageDoc.undo()
                    checkable: false
                    enabled:imageDoc.edited
                }

                Item{}

                Repeater
                {
                    model: [_colorsAction, _transfromAction, _layerAction, _filterAction]

                    ToolButton
                    {
                        action: modelData
                        display: ToolButton.IconOnly
                        flat: false
                    }
                }

                Item{}

                ToolButton
                {
                    //                    text: i18nd("mauikitimagetools","Accept")
                    icon.name: "dialog-apply"
                    onClicked: imageDoc.applyChanges()
                    enabled: !imageDoc.changesApplied

                }

            }

            DragHandler
            {
                target: _pane
                // target: _actionsBarLoader
                // grabPermissions: PointerHandler.TakeOverForbidden | PointerHandler.CanTakeOverFromAnything
                xAxis.maximum: control.width - _pane.width
                xAxis.minimum: 0

                yAxis.enabled : false

                onActiveChanged:
                {
                    if(!active)
                    {
                        console.log(centroid.position, centroid.scenePosition, centroid.velocity.x)

                        let pos = centroid.velocity.x
                        _pane.x = Qt.binding(()=> { return pos < 0 ? Maui.Style.space.big : control.width - _pane.width - Maui.Style.space.big })
                        _pane.y = Qt.binding(()=> { return control.height - _pane.height - Maui.Style.space.big })
                    }
                }
            }
        }
    }

    footBar.visible: false
    footerColumn: [

        Private.TransformationBar
        {
            id: _transBar
            visible: _actionsBarLoader.currentIndex === 1 && control.ready
            width: parent.width
        },

        Private.ColourBar
        {
            id: _colourBar
            visible: _actionsBarLoader.currentIndex === 0 && control.ready
            width: parent.width
        },

        RowLayout
        {
            id: _filtersBar
            visible: _actionsBarLoader.currentIndex === 2 && control.ready
            width: parent.width

            Row
            {
                Layout.alignment: Qt.AlignHCenter
                spacing: Maui.Style.defaultSpacing
                Button
                {
                    text: "b&w"
                    checkable: true
                    checked: false
                    onClicked:
                    {
                        if(checked)
                        {
                            editor.toGray()
                            editor.apply()
                        }else
                        {
                            editor.undo()
                        }
                    }
                }

                Button
                {
                    text: "sketch"
                    onClicked: editor.toSketch();
                }

                Button
                {
                    text: "vignette"
                    onClicked: editor.addVignette();
                }

                Button
                {
                    text: "Border"
                    property color color : "pink"
                    onClicked: editor.addBorder(100, color);
                }
            }

        }

    ]

    function selectionToolRect()
    {
        return Qt.rect(selectionTool.selectionX / editImage.ratioX,
                       selectionTool.selectionY / editImage.ratioY,
                       selectionTool.selectionWidth / editImage.ratioX,
                       selectionTool.selectionHeight / editImage.ratioY);
    }

    function crop()
    {
        console.log("CROP")
        imageDoc.crop(selectionTool.selectionX / editImage.ratioX,
                      selectionTool.selectionY / editImage.ratioY,
                      selectionTool.selectionWidth / editImage.ratioX,
                      selectionTool.selectionHeight / editImage.ratioY);
    }
}
