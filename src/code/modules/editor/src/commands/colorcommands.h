
#pragma once
#include "command.h"

#include <QImage>
#include <QRect>

/*
 * @brief ColorsCommand to perfom color operations
 */
namespace ColorCommands
{

class Brightness : public Command
{
public:
    /**
     * Contructor
     */
    Brightness(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Brightness() override = default;

    void setArea(const QRectF &area);

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
    QRectF m_area;
};

class Contrast : public Command
{
public:
    /**
     * Contructor
     */
    Contrast(QImage image, double value, const std::function<void ()> &f = nullptr);
    ~Contrast() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;


private:
    QImage m_image;
    double m_value;
    std::function<void ()> m_cb;
};

class Saturation : public Command
{
public:
    /**
     * Contructor
     */
    Saturation(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Saturation() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
};

class Hue : public Command
{
public:
    /**
     * Contructor
     */
    Hue(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Hue() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
};

class Gamma : public Command
{
public:
    /**
     * Contructor
     */
    Gamma(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Gamma() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
};

class Sharpness : public Command
{
public:
    /**
     * Contructor
     */
    Sharpness(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Sharpness() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
};


class Threshold : public Command
{
public:
    /**
     * Contructor
     */
    Threshold(QImage image, int value, const std::function<void ()> &f = nullptr);
    ~Threshold() override = default;

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;
};
}
