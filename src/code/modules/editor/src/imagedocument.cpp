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
#include "commands/transformcommand.h"
#include <QImageReader>
#include <QDebug>

ImageDocument::ImageDocument(QObject *parent)
    : QObject(parent)
{
    QImageReader::setAllocationLimit(2000);

    m_changesApplied = true;
    m_changesSaved = true;

    connect(this, &ImageDocument::pathChanged, this, [this](const QUrl &url) {
        m_image = QImage(url.isLocalFile() ? url.toLocalFile() : url.toString());
        m_originalImage = m_image;
        m_edited = false;
        m_changesApplied = true;

        Q_EMIT editedChanged();
        Q_EMIT imageChanged();
        Q_EMIT changesAppliedChanged();

        resetValues();
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

    m_changesSaved = !value;
    Q_EMIT changesSavedChanged();

    if (m_edited == value) {
        return;
    }
    m_edited = value;
    Q_EMIT editedChanged();
}

bool ImageDocument::save()
{
    applyChanges();

    if(m_originalImage.save(m_path.isLocalFile() ? m_path.toLocalFile() : m_path.toString()))
    {
        m_changesSaved = true;
        Q_EMIT changesSavedChanged();
    }

    return false;
}

bool ImageDocument::saveAs(const QUrl &location)
{
    return m_originalImage.save(location.isLocalFile() ? location.toLocalFile() : location.toString());
}

void ImageDocument::adjustBrightness(int value)
{
    if(value == m_brightness)
        return;

    auto oldValue = m_brightness;
    m_brightness = value;

    auto transformation = [val = m_brightness](QImage &ref) -> QImage
    {
        return Trans::adjustBrightness(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_brightness = oldValue;
        Q_EMIT brightnessChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

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

    auto oldValue = m_contrast;
    m_contrast = value;

    auto transformation = [val = m_contrast](QImage &ref) -> QImage
    {
        return Trans::adjustContrast(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_contrast = oldValue;
        Q_EMIT contrastChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

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

    auto oldValue = m_saturation;
    m_saturation = value;

    auto transformation = [val = m_saturation](QImage &ref) -> QImage
    {
        return Trans::adjustSaturation(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_saturation = oldValue;
        Q_EMIT saturationChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT saturationChanged();
}

void ImageDocument::adjustHue(int value)
{
    qDebug() << "adjust HUE DOCUMENT" << value;
    if(value == m_hue)
        return;

    if(m_image.isGrayscale())
        return;

    auto oldValue = m_hue;
    m_hue = value;

    auto transformation = [val = m_hue](QImage &ref) -> QImage
    {
        return Trans::adjustHue(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_hue = oldValue;
        Q_EMIT hueChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT hueChanged();
}

void ImageDocument::adjustGamma(int value)
{
    qDebug() << "adjust GAMMA DOCUMENT" << value;
    // if(m_image.isGrayscale())
    //     return;

    if(value == m_gamma)
        return;

    auto oldValue = m_gamma;
    m_gamma = value;

    auto transformation = [val = m_gamma](QImage &ref) -> QImage
    {
        return Trans::adjustGamma(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_gamma = oldValue;
        Q_EMIT gammaChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT gammaChanged();
}

void ImageDocument::adjustSharpness(int value)
{
    qDebug() << "adjust SHARPNESS DOCUMENT" << value;
    // if(m_image.isGrayscale())
    //     return;

    if(value == m_sharpness)
        return;

    auto oldValue = m_sharpness;
    m_sharpness = value;

    auto transformation = [val = m_sharpness](QImage &ref) -> QImage
    {
        return Trans::adjustSharpness(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_sharpness = oldValue;
        Q_EMIT sharpnessChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT sharpnessChanged();
}

void ImageDocument::adjustThreshold(int value)
{
    qDebug() << "adjust threshold DOCUMENT" << value;
    // if(m_image.isGrayscale())
    //     return;

    if(value == m_threshold)
        return;

    auto oldValue = m_threshold;
    m_threshold = value;

    auto transformation = [val = m_threshold](QImage &ref) -> QImage
    {
        return Trans::adjustThreshold(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_threshold = oldValue;
        Q_EMIT thresholdChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT thresholdChanged();
}

void ImageDocument::adjustGaussianBlur(int value)
{
    if(value == m_gaussianBlur)
        return;

    auto oldValue = m_gaussianBlur;
    m_gaussianBlur = value;
    auto transformation = [val = m_gaussianBlur](QImage &ref) -> QImage
    {
        qDebug() << "SXetting gaussian blur" << val;
        return Trans::adjustGaussianBlur(ref, val);
    };

    auto undoCallback = [this, oldValue]()
    {
        this->m_gaussianBlur = oldValue;
        Q_EMIT gaussianBlurChanged();
    };

    const auto command = new TransformCommand(m_image, transformation, undoCallback);
    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
    Q_EMIT gaussianBlurChanged();
}

void ImageDocument::toGray()
{
    const auto command = new TransformCommand(m_image, &Trans::toGray, nullptr);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::toBW()
{
    const auto command = new TransformCommand(m_image, &Trans::toBlackAndWhite, nullptr);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::toSketch()
{
    const auto command = new TransformCommand(m_image, &Trans::sketch, nullptr);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

void ImageDocument::addVignette()
{
    const auto command = new TransformCommand(m_image, &Trans::vignette, nullptr);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
}

auto borderTrans(int thickness, const QColor &color, QImage &ref)
{
    return Trans::addBorder(ref, thickness, color);
}

void ImageDocument::addBorder(int thickness, const QColor &color)
{
    qDebug() << "SXetting add border blur" << thickness << color << color.red() << color.green()<<color.blue();
    auto transformation = [&](QImage &ref) -> QImage
    {
        qDebug() << "SXetting add border blur2" << thickness << color;
        return Trans::addBorder(ref, thickness, color);
    };

    const auto command = new TransformCommand(m_image, transformation, nullptr);

    m_image = command->redo(m_originalImage);
    m_undos.append(command);
    setEdited(true);
    Q_EMIT imageChanged();
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

int ImageDocument::gamma() const
{
    return m_gamma;
}

int ImageDocument::sharpness() const
{
    return m_sharpness;
}

int ImageDocument::threshold() const
{
    return m_threshold;
}

int ImageDocument::gaussianBlur() const
{
    return m_gaussianBlur;
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
    m_gamma = 0;
    m_sharpness = 0;
    m_threshold = 0;
    Q_EMIT hueChanged();
    Q_EMIT saturationChanged();
    Q_EMIT brightnessChanged();
    Q_EMIT contrastChanged();
    Q_EMIT gammaChanged();
    Q_EMIT thresholdChanged();
    Q_EMIT sharpnessChanged();
}

bool ImageDocument::changesApplied() const
{
    return m_changesApplied;
}

bool ImageDocument::changesSaved() const
{
    return m_changesSaved;
}
