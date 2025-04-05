/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <QImage>
#include <QObject>
#include <QStack>
#include <QUrl>
#include <qqmlregistration.h>

#include "commands/command.h"

/**
 * @brief An ImageDocument is the base class of the ImageEditor.
 *
 * This class handles various image manipulation and contains an undo stack to allow
 * reverting the last actions. This class does not display the image, use @c ImageItem
 * for this task.
 *
 * @code{.qml}
 * KQuickImageEditor.ImageDocument {
 *    id: imageDocument
 *    path: myModel.image
 * }
 *
 * Kirigami.Actions {
 *    iconName: "object-rotate-left"
 *    onTriggered: imageDocument.rotate(-90);
 * }
 *
 * KQuickImageEditor.ImageItem {
 *     image: imageDocument.image
 * }
 * @endcode
 */
class ImageDocument : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(bool edited READ edited WRITE setEdited NOTIFY editedChanged)

    Q_PROPERTY(int brightness READ brightness NOTIFY brightnessChanged FINAL)

public:
    ImageDocument(QObject *parent = nullptr);
    // ~ImageDocument() override = default;

    /**
     * The image was is displayed. This propriety is updated when the path change
     * or commands are applied.
     *
     * @see imageChanged
     */
    QImage image() const;

    /**
     * This propriety store if the document was changed or not.
     *
     * @see setEdited
     * @see editedChanged
     */
    bool edited() const;

    /**
     * Change the edited value.
     * @param value The new value.
     */
    void setEdited(bool value);

    QUrl path() const;
    void setPath(const QUrl &path);

    /**
     * Rotate the image.
     * @param angle The angle of the rotation in degree.
     */
    Q_INVOKABLE void rotate(int angle);

    /**
     * Mirror the image.
     * @param horizontal Mirror the image horizontally.
     * @param vertical Mirror the image vertically.
     */
    Q_INVOKABLE void mirror(bool horizontal, bool vertical);

    /**
     * Crop the image.
     * @param x The x coordinate of the new image in the old image.
     * @param y The y coordinate of the new image in the old image.
     * @param width The width of the new image.
     * @param height The height of the new image.
     */
    Q_INVOKABLE void crop
        (int x, int y, int width, int height);

    /**
     * Resize the image.
     * @param width The width of the new image.
     * @param height The height of the new image.
     */
    Q_INVOKABLE void resize(int width, int height);

    /**
     * Undo the last edit on the images.
     */
    Q_INVOKABLE void undo();

    /**
     * Cancel all the edit.
     */
    Q_INVOKABLE void cancel();

    /**
     * Save current edited image in place. This is a destructive operation and can't be reverted.
     * @return true iff the file saving operation was successful.
     */
    Q_INVOKABLE bool save();

    /**
     * Save current edited image as a new image.
     * @param location The location where to save the new image.
     * @return true iff the file saving operattion was successful.
     */
    Q_INVOKABLE bool saveAs(const QUrl &location);

    Q_INVOKABLE void adjustBrightness(int value);// between -100 and 100
    Q_INVOKABLE void adjustContrast(double value); //between 1.0 and 3.0
    Q_INVOKABLE void adjustSaturation(double value); //between -255 and 255

    Q_INVOKABLE void applyChanges();

    int brightness() const;

Q_SIGNALS:
    void pathChanged(const QUrl &url);
    void imageChanged();
    void editedChanged();
    void brightnessChanged();
    void contrastChanged();

private:
    QUrl m_path;
    QStack<Command *> m_undos;
    QImage m_image;
    QImage m_originalImage;
    bool m_edited;
    int m_brightness = 0;
    double m_contrast = 0;
};
