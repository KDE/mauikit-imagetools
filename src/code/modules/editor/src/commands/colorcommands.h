
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

    QImage redo(QImage image) override;
    QImage undo(QImage image) override;

private:
    QImage m_image;
    int m_value;
    std::function<void ()> m_cb;

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


}
