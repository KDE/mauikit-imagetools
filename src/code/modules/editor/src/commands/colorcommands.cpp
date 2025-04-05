#include "colorcommands.h"
#include <preprocessimage.hpp>
#include <convertimage.hpp>
#include <cvmatandqimage.h>

using namespace ColorCommands;

Brightness::Brightness(QImage image, int value, const std::function<void ()> &f)
    : m_image(image)
    , m_value(value)
// ,m_cb(f)
{
    m_cb = f;
    qDebug() << "create brightness <<" << m_value;
}

QImage Brightness::undo(QImage image)
{
    Q_UNUSED(image)

    if(m_cb != nullptr)
    {
        m_cb();
    }

    qDebug() << "back to birghtness" << m_value;
    return m_image;
}

QImage Brightness::redo(QImage image)
{
    // m_image = image;

    if(m_value > 100)
        m_value = 100;

    if(m_value < -100)
        m_value = -100;

    auto m_imgMat = QtOcv::image2Mat(image);
    auto newMat = PreprocessImage::adjustBrightness(m_imgMat, m_value);
    // qDebug() << "IS PROCESSED IMAGE VALIUD" << img.isNull() <<  img.format();

    auto img = QtOcv::mat2Image(newMat); //remember to delete

    qDebug() << "testing saturation" << image.format() << img.format()<< img.isDetached()<<newMat.rows << newMat.cols << newMat.step << newMat.empty();

           return img;
}

Contrast::Contrast(QImage image, double value, const std::function<void ()> &f)
    : m_image(image)
    , m_value(value)
{
    m_cb = f;
}

QImage Contrast::redo(QImage image)
{
    if(m_value > 3.0)
        m_value = 3.0;

    if(m_value < 1.0)
        m_value = 1.0;

    auto m_imgMat = QtOcv::image2Mat(image);
    auto newMat = PreprocessImage::adjustContrast(m_imgMat, m_value);
    auto img = QtOcv::mat2Image(newMat); //remember to delete
    // qDebug() << "IS PROCESSED IMAGE VALIUD" << img.isNull() <<  img.format();

    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();

    return img;
}

QImage Contrast::undo(QImage image)
{
    Q_UNUSED(image)

    if(m_cb != nullptr)
    {
        m_cb();
    }

    qDebug() << "back to birghtness" << m_value;
    return m_image;
}

Saturation::Saturation(QImage image, int value, const std::function<void ()> &f)
    : m_image(image)
    , m_value(value)
{
    m_cb = f;
}

QImage Saturation::redo(QImage image)
{
    // m_image = image;

    if(m_value > 100)
        m_value = 100;

    if(m_value < -100)
        m_value = -100;

    auto m_imgMat = QtOcv::image2Mat(image);
    auto newMat = PreprocessImage::adjustSaturation(m_imgMat, m_value);
    // qDebug() << "IS PROCESSED IMAGE VALIUD" << img.isNull() <<  img.format();

    auto img = QtOcv::mat2Image(newMat); //remember to delete

    qDebug() << "testing saturation" << image.format() << img.format()<< img.isDetached()<<newMat.rows << newMat.cols << newMat.step << newMat.empty();

    return img;
}

QImage Saturation::undo(QImage image)
{
    Q_UNUSED(image)

    if(m_cb != nullptr)
    {
        m_cb();
    }

    qDebug() << "back to birghtness" << m_value;
    return m_image;
}
