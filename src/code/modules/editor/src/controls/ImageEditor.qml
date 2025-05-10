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
Maui.PageLayout
{
    id: control

    property url url

    readonly property bool ready : String(control.url).length
    
    readonly property alias editor : imageDoc

    property Item middleContentBar : null

    property int preferredBorderThickness : 80
    property string preferredBorderColor : "white"

    signal saved()
    signal savedAs(string url)
    signal canceled()

    Component
    {
        id: _boderDialogComponent
        Maui.InfoDialog
        {
            onClosed: destroy()
            standardButtons: Dialog.Apply | Dialog.Cancel
            message: i18n("Select a color and thickness for the border effect.")

            Maui.FlexSectionItem
            {
                label1.text: i18n("Border color")

                Maui.ColorsRow
                {
                    id: _colorsRow
                    currentColor: control.preferredBorderColor
                    defaultColor: "white"
                    colors: ["white", "black", "grey", "pink", "violet", "green", "blue", "yellow"]
                    onColorPicked: (color) => control.preferredBorderColor = color
                }
            }

            Maui.FlexSectionItem
            {
                label1.text: i18n("Border thicknes")

                SpinBox
                {
                    id: _spinBox
                    from: 1
                    to: 200
                    value: control.preferredBorderThickness
                    onValueChanged: control.preferredBorderThickness = value
                }
            }

            onApplied:
            {
                imageDoc.addBorder(control.preferredBorderThickness, control.preferredBorderColor)
                close()
            }

            onRejected: close()
        }
    }

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

    // headBar.visible: control.ready
    // headBar.background: null

    // altHeader: split
    splitIn: ToolBar.Footer
    splitSection: Maui.PageLayout.Section.Middle
    split: true

    // footerMargins: Maui.Style.defaultPadding
    rightContent: Button
    {
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

    middleContent: control.middleContentBar

    leftContent: [
        Button
        {
            icon.name: "go-previous"
            text: "Save"
            enabled: imageDoc.edited
            Maui.Controls.status : imageDoc.edited ? Maui.Controls.Positive : Maui.Controls.Normal

            onClicked:
            {
                imageDoc.save()
                control.saved()
            }
        },

        ToolButton
        {
            icon.name: "document-save-as"
            enabled: imageDoc.edited
            onClicked:
            {
                imageDoc.saveAs()
                control.savedAs()
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
        onTriggered:
        {
            _actionsBarLoader.currentIndex = 0
            control.middleContentBar = _colourBar.bar
        }
    }

    Action
    {
        id: _transfromAction
        icon.name: "dialog-transform"
        text: i18nd("mauikitimagetools","Transform")
        checked: _actionsBarLoader.currentIndex === 1
        onTriggered:
        {
            _actionsBarLoader.currentIndex = 1
            control.middleContentBar = _transBar.bar
        }
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
        onTriggered:
        {
            _actionsBarLoader.currentIndex = 2
            control.middleContentBar = effectBar
        }
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

    // footBar.visible: false
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
        }
    ]


    property Item effectBar :  Row
    {
        Layout.alignment: Qt.AlignHCenter
        spacing: Maui.Style.defaultSpacing
        Button
        {
            text: "gray"
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
            text: "bw"
            onClicked: editor.toBW();
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

        Maui.ToolActions
        {
            checkable: false
            autoExclusive: false


            Action
            {

                text: i18n("Border")
                onTriggered: editor.addBorder(preferredBorderThickness, preferredBorderColor);
            }

            Action
            {
                icon.name: "configuration"
                onTriggered:
                {
                    var dialog = _boderDialogComponent.createObject(this)
                    dialog.open()
                }
            }
        }
    }


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
