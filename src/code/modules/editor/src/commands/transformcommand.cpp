#include "transformcommand.h"
#include "opencv2/photo.hpp"
#include <preprocessimage.hpp>
#include <convertimage.hpp>
#include <cvmatandqimage.h>

TransformCommand::TransformCommand(QImage image, Transformation trans, const std::function<void ()> &undo)
    : m_image(image)
    ,m_transform(trans)
    ,m_cb(undo)
{

}

QImage TransformCommand::redo(QImage image)
{
    if(m_transform != nullptr)
    {
        return m_transform(image);
    }

    return QImage{};
}

QImage TransformCommand::undo(QImage image)
{
    Q_UNUSED(image)

    if(m_cb != nullptr)
    {
        m_cb();
    }
    return m_image;
}


QImage Trans::toGray(QImage &ref)
{
    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::grayscale(m_imgMat);
    auto img = QtOcv::mat2Image(newMat);
    return img;
}

QImage Trans::sketch(QImage &ref)
{
    auto m_imgMat = QtOcv::image2Mat(ref);
    cv::Mat out;
    cv::Mat out2;

    if(m_imgMat.channels() == 1)
    {
        cv::cvtColor(m_imgMat, m_imgMat, cv::COLOR_GRAY2BGR);
    }

           // cv::Mat im(m_imgMat);
           // cv::GaussianBlur(m_imgMat, out, cv::Size(5,5), 0, 0);
           // cv::pencilSketch(m_imgMat, out2, im, 10);
           // cv::edgePreservingFilter(m_imgMat, out, cv::RECURS_FILTER);
           // cv::detailEnhance(m_imgMat, out);
    cv::stylization(m_imgMat, out);
                                    //(Mat src, Mat dst, int flags=1, float sigma_s=60, float sigma_r=0.4f)
    auto img = QtOcv::mat2Image(out);
    return img;
}

QImage Trans::adjustGaussianBlur(QImage &ref, int value)
{
    auto m_imgMat = QtOcv::image2Mat(ref);
    cv::Mat out;

    cv::GaussianBlur(m_imgMat, out, cv::Size(), value);

    auto img = QtOcv::mat2Image(out);
    return img;
}

QImage Trans::adjustContrast(QImage &ref, int value)
{
    if(value > 100)
        value = 100;
    else if(value < -100)
        value = -100;

    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::adjustContrast(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat); //remember to delete
    // qDebug() << "IS PROCESSED IMAGE VALIUD" << img.isNull() <<  img.format();

    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();

    return img;
}

QImage Trans::adjustBrightness(QImage &ref, int value)
{
    if(value > 255)
        value = 255;

    else if(value < -255)
        value = -255;

    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::adjustBrightness(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat);
    return img;
}

QImage Trans::adjustSaturation(QImage &ref, int value)
{
    if(value > 100)
        value = 100;

    if(value < -100)
        value = -100;

    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::adjustSaturation(m_imgMat, value);

    auto img = QtOcv::mat2Image(newMat);
    return img;
}

QImage Trans::adjustHue(QImage &ref, int value)
{
    if(value > 180)
        value = 180;
    else if(value < 0)
        value = 0;
    qDebug() << "Creating command for hue" << value;

    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::hue(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat); //remember to delete
    // qDebug() << "IS PROCESSED IMAGE VALIUD" << img.isNull() <<  img.format();

    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();

    return img;
}

QImage Trans::adjustGamma(QImage &ref, int value)
{
    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::gamma(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat);

    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();
    return img;
}

QImage Trans::adjustSharpness(QImage &ref, int value)
{
    qDebug() << "Creating command for sharpness" << value;

    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::sharpness(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat);
    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();
    return img;
}

QImage Trans::adjustThreshold(QImage &ref, int value)
{
    auto m_imgMat = QtOcv::image2Mat(ref);
    auto newMat = PreprocessImage::manualThreshold(m_imgMat, value);
    auto img = QtOcv::mat2Image(newMat);
    qDebug() << m_imgMat.rows << m_imgMat.cols << m_imgMat.step << m_imgMat.empty();
    return img;
}

static double dist(cv::Point a, cv::Point b)
{
    return sqrt(pow((double) (a.x - b.x), 2) + pow((double) (a.y - b.y), 2));
}

// Helper function that computes the longest distance from the edge to the center point.
static double getMaxDisFromCorners(const cv::Size& imgSize, const cv::Point& center)
{
    // given a rect and a line
    // get which corner of rect is farthest from the line

    std::vector<cv::Point> corners(4);
    corners[0] = cv::Point(0, 0);
    corners[1] = cv::Point(imgSize.width, 0);
    corners[2] = cv::Point(0, imgSize.height);
    corners[3] = cv::Point(imgSize.width, imgSize.height);

    double maxDis = 0;
    for (int i = 0; i < 4; ++i)
    {
        double dis = dist(corners[i], center);
        if (maxDis < dis)
            maxDis = dis;
    }

    return maxDis;
}

static void generateGradient(cv::Mat& mask)
{
    cv::Point firstPt = cv::Point(mask.size().width/2, mask.size().height/2);
    double radius = 1.0;
    double power = 0.8;

    double maxImageRad = radius * getMaxDisFromCorners(mask.size(), firstPt);

    mask.setTo(cv::Scalar(1));
    for (int i = 0; i < mask.rows; i++)
    {
        for (int j = 0; j < mask.cols; j++)
        {
            double temp = dist(firstPt, cv::Point(j, i)) / maxImageRad;
            temp = temp * power;
            double temp_s = pow(cos(temp), 4);
            mask.at<double>(i, j) = temp_s;
        }
    }
}

QImage Trans::vignette(QImage &ref)
{
    auto mat = QtOcv::image2Mat(ref);

    cv::Mat maskImg(mat.size(), CV_64F);
    generateGradient(maskImg);

    cv::Mat labImg(mat.size(), CV_8UC3);
    cv::cvtColor(mat, labImg, cv::COLOR_BGR2Lab);

    for (int row = 0; row < labImg.size().height; row++)
    {
        for (int col = 0; col < labImg.size().width; col++)
        {
            cv::Vec3b value = labImg.at<cv::Vec3b>(row, col);
            value.val[0] *= maskImg.at<double>(row, col);
            labImg.at<cv::Vec3b>(row, col) =  value;
        }
    }

    cv::Mat output;
    cv::cvtColor(labImg, output, cv::COLOR_Lab2BGR);
    auto img = QtOcv::mat2Image(output);
    return img;
}
