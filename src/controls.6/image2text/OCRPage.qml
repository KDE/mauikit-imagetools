// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13

import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3
import QtQuick.Window 2.13

import org.mauikit.controls 1.3 as Maui
import org.mauikit.imagetools 1.3 as IT
import org.kde.kquickimageeditor 1.0 as KQuickImageEditor


Maui.Page
{
    id: control
    
    property alias url : _ocr.filePath
    
    IT.OCR
    {
        id: _ocr
    }
    
    headBar.rightContent: Maui.ToolButtonMenu
    {
        icon.name: "format-text-bold"
        
        
        MenuItem
        {
            text: i18n("Read Area")
            icon.name: "transform-crop"
            onTriggered:
            {
                
                control.push(_ocrComponent)
                
            }
        }
        
        MenuItem
        {
            text: i18n("Read All")
            icon.name: "viewimage"
            onTriggered:
            {
                console.log(_ocr.getText())
            }
        }
        
        MenuSeparator{}
        
        MenuItem
        {
            text: i18n("Configure")
        }
    }
    
    Maui.SplitView
    {
        anchors.fill: parent
        
        Maui.SplitViewItem
        {
            autoClose: false
            SplitView.fillHeight: true
            SplitView.fillWidth: true
            
            KQuickImageEditor.ImageItem
            {
                id: editImage
                anchors.fill: parent
                
                readonly property real ratioX: editImage.paintedWidth / editImage.nativeWidth;
                readonly property real ratioY: editImage.paintedHeight / editImage.nativeHeight;
                
                fillMode: KQuickImageEditor.ImageItem.PreserveAspectFit
                image: imageDoc.image
                
                KQuickImageEditor.ImageDocument
                {
                    id: imageDoc
                    path: control.url
                }
                
                KQuickImageEditor.SelectionTool
                {
                    id: selectionTool
                    visible: true
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
                            _ocr.area = Qt.rect(selectionTool.selectionX / editImage.ratioX,
                                                selectionTool.selectionY / editImage.ratioY,
                                                selectionTool.selectionWidth / editImage.ratioX,
                                                selectionTool.selectionHeight / editImage.ratioY)
                            
                            
                            _listModel.append({'text': _ocr.getText()})
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
        }
        
        Maui.SplitViewItem
        {
            autoClose: false
            visible: _textArea.text
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.maximumWidth: 400
            SplitView.maximumHeight: 400
            SplitView.preferredWidth: 400
            SplitView.preferredHeight: 400
            
            Maui.Page
            {
                anchors.fill: parent
                Maui.Theme.colorSet: Maui.Theme.Window
                
                ListModel { id: _listModel}
                
                Maui.ListBrowser
                {
                    id: _textArea
                    anchors.fill: parent
                    
                    model: _listModel
                    
                    delegate: ItemDelegate
                    {
                        id: _delegate
                        text: model.text
                        width: ListView.view.width
                        onClicked: Maui.Handy.copyTextToClipboard(model.text)
                        
                        background: Rectangle
                        {
                            radius: Maui.Style.radiusV
                            color: Maui.Theme.alternateBackgroundColor
                        }
                        
                        contentItem: TextArea
                        {
                            text: _delegate.text
                        }
                    }
                }
            }
        }
        
    }
    
}
