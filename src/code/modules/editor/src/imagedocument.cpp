/*
 * SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "imagedocument.h"

#include "commands/cropcommand.h"
#include "commands/mirrorcommand.h"
#include "commands/resizecommand.h"
#include "commands/rotatecommand.h"
#include "commands/colorcommands.h"
#include <QDebug>

ImageDocument::ImageDocument(QObject *parent)
    : QObject(parent)
{
    m_changesApplied = true;

    connect(this, &ImageDocument::pathChanged, this, [this](const QUrl &url) {
        m_image = QImage(url.isLocalFile() ? url.toLocalFile() : url.toString());
        m_originalImage = m_image;
        m_edited = false;
        Q_EMIT editedChanged();
        Q_EMIT imageChanged();
    });
}

void ImageDocument::cancel()
{
    while (!m_undos.empty()) {
        const auto command = m_undos.pop();
        if(m_undos.isEmpty())
            m_image = command->undo(m_image);
        delete command;
    }

    resetValues();
    setEdited(false);
    Q_EMIT imageChanged();
}

QImage ImageDocument::image() const
{
    return m_image;
}

bool ImageDocument::edited() const
{
    return m_edited;
}

void ImageDocument::undo()
{
    if(m_undos.empty())
    {
        qDebug() << "No more commands to undo";
        return;
    }

    const auto command = m_undos.pop();
    m_image = command->undo();
    m_originalImage = m_image;
    delete command;
    Q_EMIT imageChanged();
    if (m_undos.empty()) {
        setEdited(false);
    }
}

void ImageDocument::crop(int x, int y, int width, int height)
{
    const auto command = new CropCommand(QRect(x, y, width, height));
    m_image = command->redo(m_image);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::resize(int width, int height)
{
    const auto command = new ResizeCommand(QSize(width, height));
    m_image = command->redo(m_image);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::mirror(bool horizontal, bool vertical)
{
    const auto command = new MirrorCommand(horizontal, vertical);
    m_image = command->redo(m_image);

    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::rotate(int angle)
{
    QTransform transform;
    transform.rotate(angle);
    const auto command = new RotateCommand(transform);
    m_image = command->redo(m_image);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::setEdited(bool value)
{



    m_changesApplied = !value;
    Q_EMIT changesAppliedChanged();

    if (m_edited == value) {
        return;
    }
    m_edited = value;
    Q_EMIT editedChanged();
}

bool ImageDocument::save()
{
    return m_originalImage.save(m_path.isLocalFile() ? m_path.toLocalFile() : m_path.toString());
}

bool ImageDocument::saveAs(const QUrl &location)
{
    return m_originalImage.save(location.isLocalFile() ? location.toLocalFile() : location.toString());
}

void ImageDocument::adjustBrightness(int value)
{
    if(value == m_brightness)
        return;

    auto oldBrightness = m_brightness;
    m_brightness = value;
    const auto command = new ColorCommands::Brightness(m_image, m_brightness, [this, oldBrightness]()
                                                       {
                                                           this->m_brightness = oldBrightness;
                                                           Q_EMIT brightnessChanged();
                                                       });
    // command->setArea({0,0, 200, 200});
    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    Q_EMIT brightnessChanged();
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::adjustContrast(int value)
{
    if(value == m_contrast)
        return;

    auto oldContrast = m_contrast;
    m_contrast = value;
    const auto command = new ColorCommands::Contrast(m_image, m_contrast, [this, oldContrast]()
                                                     {
                                                         this->m_contrast = oldContrast;
                                                         Q_EMIT contrastChanged();
                                                     });

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT contrastChanged();
    Q_EMIT imageChanged();
}

void ImageDocument::adjustSaturation(int value)
{
    if(m_image.isGrayscale())
        return;

    if(value == m_saturation)
        return;

    auto oldSaturation = m_saturation;
    m_saturation = value;
    const auto command = new ColorCommands::Saturation(m_image, m_saturation, [this, oldSaturation]()
                                                       {
                                                           this->m_saturation = oldSaturation;
                                                           Q_EMIT saturationChanged();
                                                       });

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT saturationChanged();
}

void ImageDocument::adjustHue(int value)
{
    qDebug() << "adjust HUE DOCUMENT" << value;
    if(m_image.isGrayscale())
        return;

    if(value == m_hue)
        return;

    auto oldValue = m_hue;
    m_hue = value;
    const auto command = new ColorCommands::Hue(m_image, m_hue, [this, oldValue]()
                                                       {
                                                           this->m_hue = oldValue;
                                                           Q_EMIT hueChanged();
                                                       });

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT saturationChanged();
}

void ImageDocument::applyChanges()
{
    resetValues();

    m_originalImage = m_image;
    m_changesApplied = true;
    Q_EMIT changesAppliedChanged();
}

int ImageDocument::brightness() const
{
    return m_brightness;
}

int ImageDocument::contrast() const
{
    return m_contrast;
}

int ImageDocument::saturation() const
{
    return m_saturation;
}

int ImageDocument::hue() const
{
    return m_hue;
}

QUrl ImageDocument::path() const
{
    return m_path;
}

void ImageDocument::setPath(const QUrl &path)
{
    m_path = path;
    Q_EMIT pathChanged(path);
}

// #include "moc_imagedocument.cpp"

QRectF ImageDocument::area() const
{
    return m_area;
}

void ImageDocument::setArea(const QRectF &newArea)
{
    if (m_area == newArea)
        return;
    m_area = newArea;
    Q_EMIT areaChanged();
}

void ImageDocument::resetArea()
{
    setArea({}); // TODO: Adapt to use your actual default value
}

void ImageDocument::resetValues()
{
    m_contrast = 0;
    m_brightness = 0;
    m_saturation = 0;
    m_hue = 0;
    Q_EMIT hueChanged();
    Q_EMIT saturationChanged();
    Q_EMIT brightnessChanged();
    Q_EMIT contrastChanged();
}

bool ImageDocument::changesApplied() const
{
    return m_changesApplied;
}
