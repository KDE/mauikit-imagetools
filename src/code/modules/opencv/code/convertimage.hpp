/*****************************************************************************
 * convertimage.hpp
 *
 * Created: 5/28/2020 2020 by mguludag
 *
 * Copyright 2020 mguludag. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QImage>
#include <QObject>
#include "opencvlib_export.h"

inline unsigned char float2uchar_bounded(const float &in)
{
    return (in < 0.0f) ? 0 : ((in > 255.0f) ? 255 : static_cast<unsigned char>(in));
}

class OPENCVLIB_EXPORT ConvertImage
{
public:
    ConvertImage();
    static cv::Mat qimageToMatRef(QImage &img, int format)
    {
        return cv::Mat(img.height(),
                       img.width(),
                       format,
                       img.bits(),
                       static_cast<size_t>(img.bytesPerLine()));
    }
    static cv::Mat qimageToMat(QImage img, int format)
    {
        return cv::Mat(img.height(),
                       img.width(),
                       format,
                       img.bits(),
                       static_cast<size_t>(img.bytesPerLine()));
    }
    static QImage matToQimageRef(cv::Mat &mat, QImage::Format format)
    {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);
    }
    static QImage matToQimage(cv::Mat mat, QImage::Format format)
    {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);
    }

    static cv::Mat readImage(const QString &fileUrl)
    {
        cv::Mat res2;
        cv::Mat res = cv::imread(fileUrl.toStdString(), 0);
         res.convertTo(res,CV_32FC1,1.0/255.0);
        // res.cvtColor(res, res, cv::COLOR_BGR2GRAY);
            // cv::threshold(res, res2,127,255, cv::THRESH_BINARY|cv::THRESH_OTSU);
        return res;
    }
};
