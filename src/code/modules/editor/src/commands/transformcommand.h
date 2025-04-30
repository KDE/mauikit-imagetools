#pragma once
#include "command.h"

typedef const std::function<QImage (QImage&)> & Transformation;
template< class T>
using TransformationVal = const  std::function<QImage (QImage&, T value)> & ;


class TransformCommand : public Command
{
public:
    TransformCommand(QImage image, Transformation trans = nullptr, const std::function<void ()> &undo = nullptr);
    ~TransformCommand() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    Transformation m_transform;
    std::function<void ()> m_cb;
};

namespace Trans
{
QImage toGray(QImage &ref);
QImage sketch(QImage &ref);
QImage adjustGaussianBlur(QImage &ref, int value);
QImage adjustContrast(QImage &ref, int value);
QImage adjustBrightness(QImage &ref, int value);
QImage adjustSaturation(QImage &ref, int value);
QImage adjustHue(QImage &ref, int value);
QImage adjustGamma(QImage &ref, int value);
QImage adjustSharpness(QImage &ref, int value);
QImage adjustThreshold(QImage &ref, int value);
QImage vignette(QImage &ref);
QImage addBorder(QImage &ref, int thickness, const QColor &color);
};
