/*****************************************************************************
 * preprocessimage.cpp
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
#include "preprocessimage.hpp"
#include <opencv2/photo.hpp>
#include "opencv2/opencv.hpp"

// Global Constants
const int bgr_max = 255; // max value of a bgr pixel
const int bgr_half = 128; // middle value of a bgr pixel
std::vector<cv::Point> PreprocessImage::poi = {};

PreprocessImage::PreprocessImage()
{
    CV_DNN_REGISTER_LAYER_CLASS(Crop, MyCropLayer);
}

cv::Mat PreprocessImage::deskewRotate(cv::Mat &image)
{
    if (!image.empty()) {
        cv::Mat orig = image.clone();
        cv::Mat warped;
        fourPointTransform(orig, warped, poi);
        cvtColor(warped, warped, cv::COLOR_RGB2BGRA);
        return warped;
    } else {
        return image;
    }
}

void PreprocessImage::adaptThreshold(cv::Mat &image,
                                     bool isMorphOp,
                                     uint8_t morphKernel,
                                     uint8_t blurValue)
{
    if (!image.empty()) {
        cvtColor(image, image, cv::COLOR_BGRA2GRAY);

        adaptiveThreshold(image, image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 9, 14);

        if (isMorphOp) {
            cv::dilate(image,
                       image,
                       cv::getStructuringElement(cv::MORPH_RECT,
                                                 cv::Size(morphKernel, morphKernel)));
            cv::erode(image,
                      image,
                      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morphKernel, morphKernel)));
        }

        GaussianBlur(image, image, cv::Size(blurValue, blurValue), 0);
        cvtColor(image, image, cv::COLOR_GRAY2BGRA);
    }
}

void PreprocessImage::toGray(cv::Mat &image, uint8_t blurValue)
{
    if (!image.empty()) {
        cvtColor(image, image, cv::COLOR_BGR2GRAY);
        GaussianBlur(image, image, cv::Size(blurValue, blurValue), 0);
        cvtColor(image, image, cv::COLOR_GRAY2BGR);
    }
}

cv::Mat PreprocessImage::grayscale(cv::Mat matrix)
{
    cvtColor(matrix, matrix, cv::COLOR_BGR2GRAY);
    return matrix;
}

bool PreprocessImage::compareContourAreas(std::vector<cv::Point> &contour1,
                                          std::vector<cv::Point> &contour2)
{
    double i = fabs(contourArea(cv::Mat(contour1)));
    double j = fabs(contourArea(cv::Mat(contour2)));
    return (i > j);
}

bool PreprocessImage::compareXCords(cv::Point &p1, cv::Point &p2)
{
    return (p1.x < p2.x);
}

bool PreprocessImage::compareYCords(cv::Point &p1, cv::Point &p2)
{
    return (p1.y < p2.y);
}

bool PreprocessImage::compareDistance(std::pair<cv::Point, cv::Point> &p1,
                                      std::pair<cv::Point, cv::Point> &p2)
{
    return (cv::norm(p1.first - p1.second) < cv::norm(p2.first - p2.second));
}

double PreprocessImage::_distance(cv::Point &p1, cv::Point &p2)
{
    return sqrt(((p1.x - p2.x) * (p1.x - p2.x)) + ((p1.y - p2.y) * (p1.y - p2.y)));
}

void PreprocessImage::resizeToHeight(cv::Mat &src, cv::Mat &dst, int height)
{
    cv::Size2d s = cv::Size2d(src.cols * (height / double(src.rows)), height);
    cv::resize(src, dst, s, cv::INTER_AREA);
}

void PreprocessImage::orderPoints(std::vector<cv::Point> &inpts, std::vector<cv::Point> &ordered)
{
    sort(inpts.begin(), inpts.end(), &compareXCords);
    std::vector<cv::Point> lm(inpts.begin(), inpts.begin() + 2);
    std::vector<cv::Point> rm(inpts.end() - 2, inpts.end());

    sort(lm.begin(), lm.end(), &compareYCords);
    cv::Point tl(lm[0]);
    cv::Point bl(lm[1]);
    std::vector<std::pair<cv::Point, cv::Point>> tmp;
    for (size_t i = 0; i < rm.size(); i++) {
        tmp.push_back(std::make_pair(tl, rm[i]));
    }

    sort(tmp.begin(), tmp.end(), &compareDistance);
    cv::Point tr(tmp[0].second);
    cv::Point br(tmp[1].second);

    ordered.push_back(tl);
    ordered.push_back(tr);
    ordered.push_back(br);
    ordered.push_back(bl);
}

void PreprocessImage::fourPointTransform(cv::Mat &src, cv::Mat &dst, std::vector<cv::Point> &pts)
{
    std::vector<cv::Point> ordered_pts;
    orderPoints(pts, ordered_pts);

    std::vector<cv::Point2f> src_;
    std::vector<cv::Point2f> dst_;

    double wa = _distance(ordered_pts[2], ordered_pts[3]);
    double wb = _distance(ordered_pts[1], ordered_pts[0]);
    double mw = fmax(wa, wb);

    double ha = _distance(ordered_pts[1], ordered_pts[2]);
    double hb = _distance(ordered_pts[0], ordered_pts[3]);
    double mh = fmax(ha, hb);

    src_ = {
        cv::Point(ordered_pts[0].x, ordered_pts[0].y),
        cv::Point(ordered_pts[1].x, ordered_pts[1].y),
        cv::Point(ordered_pts[2].x, ordered_pts[2].y),
        cv::Point(ordered_pts[3].x, ordered_pts[3].y),
    };
    dst_ = {cv::Point(0, 0),
            cv::Point(static_cast<int>(mw) - 1, 0),
            cv::Point(static_cast<int>(mw) - 1, static_cast<int>(mh) - 1),
            cv::Point(0, static_cast<int>(mh) - 1)};
    cv::Mat m = getPerspectiveTransform(src_, dst_);
    cv::warpPerspective(src, dst, m, cv::Size(static_cast<int>(mw), static_cast<int>(mh)));
}

std::vector<cv::Point2f> PreprocessImage::getPoints(cv::Mat &src)
{
    cv::Mat gray, edged;
    std::vector<cv::Point2f> src_;
    std::vector<cv::Point2f> dst_;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy = {};
    std::vector<std::vector<cv::Point>> approx;

    if (!src.empty()) {
        double ratio = src.rows / 500.0;
        preProcess(src, edged);
        cv::findContours(edged, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        approx.resize(contours.size());
        size_t i, j;
        for (i = 0; i < contours.size(); i++) {
            double peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approx[i], 0.03 * peri, true);
        }

        sort(approx.begin(), approx.end(), compareContourAreas);

        for (i = 0; i < approx.size(); i++) {
            if (approx[i].size() == 4) {
                break;
            }
        }

        if (i < approx.size()) {
            for (j = 0; j < approx[i].size(); j++) {
                approx[i][j] *= ratio;
            }

            std::vector<cv::Point> ordered_pts;
            orderPoints(approx[i], ordered_pts);

            double wa = _distance(ordered_pts[2], ordered_pts[3]);
            double wb = _distance(ordered_pts[1], ordered_pts[0]);
            double mw = fmax(wa, wb);

            double ha = _distance(ordered_pts[1], ordered_pts[2]);
            double hb = _distance(ordered_pts[0], ordered_pts[3]);
            double mh = fmax(ha, hb);

            src_ = {
                cv::Point(ordered_pts[0].x, ordered_pts[0].y),
                cv::Point(ordered_pts[1].x, ordered_pts[1].y),
                cv::Point(ordered_pts[2].x, ordered_pts[2].y),
                cv::Point(ordered_pts[3].x, ordered_pts[3].y),
            };
            dst_ = {cv::Point(0, 0),
                    cv::Point(static_cast<int>(mw) - 1, 0),
                    cv::Point(static_cast<int>(mw) - 1, static_cast<int>(mh) - 1),
                    cv::Point(0, static_cast<int>(mh) - 1)};
        }
    }
    return src_;
}

void PreprocessImage::setPoints(std::vector<cv::Point2f> pt)
{
    poi.clear();
    for (auto i : pt) {
        poi.push_back(i);
    }
}

void PreprocessImage::preProcess(cv::Mat &src, cv::Mat &dst)
{
    cv::Mat imageGrayed;
    cv::Mat imageOpen, imageClosed, imageBlurred;

    cv::cvtColor(src, imageGrayed, cv::COLOR_BGRA2GRAY);

    cv::adaptiveThreshold(imageGrayed,
                          imageGrayed,
                          255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY,
                          25,
                          4);

    cv::Mat structuringElmt = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
    cv::morphologyEx(imageGrayed, imageOpen, cv::MORPH_OPEN, structuringElmt);
    cv::morphologyEx(imageOpen, imageClosed, cv::MORPH_CLOSE, structuringElmt);

    cv::medianBlur(imageClosed, imageBlurred, 9);

    cv::Canny(imageBlurred, dst, 200, 250);
}

double PreprocessImage::computeSkew(cv::Mat src)
{
    cv::cvtColor(src, src, cv::COLOR_BGRA2GRAY);
    cv::Size size = src.size();
    cv::threshold(src, src, 180, 255, cv::THRESH_OTSU);
    cv::bitwise_not(src, src);
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(src, lines, 1, CV_PI / 180, 100, size.width / 2., 10);
    cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = static_cast<unsigned>(lines.size());
    for (unsigned i = 0; i < nb_lines; ++i) {
        cv::line(disp_lines,
                 cv::Point(lines[i][0], lines[i][1]),
                 cv::Point(lines[i][2], lines[i][3]),
                 cv::Scalar(255, 0, 0));
        angle += atan2(static_cast<double>(lines[i][3]) - lines[i][1],
                       static_cast<double>(lines[i][2]) - lines[i][0]);
    }
    angle /= nb_lines;
    return angle * 180 / CV_PI;
}

cv::Mat PreprocessImage::adjustBrightness(cv::Mat &in, int value, cv::Rect rect)
{
    if (!in.empty())
    {
        cv::Mat out;
        const bool is_inside = (rect & cv::Rect(0, 0, in.cols, in.rows)) == rect;

        if(is_inside && rect.area() > 0)
        {
            cv::Mat out2(in);
            cv::Mat roi(in(rect));

            roi.convertTo(out, -1, 1, value);
            out.copyTo(out2(rect));
            return out2;
        }else
        {
            in.convertTo(out, -1, 1, value);
            return out;
        }

    } else
        return in;
}

cv::Mat PreprocessImage::adjustContrast(cv::Mat &in, int beta)
{
    if (!in.empty()) {
        cv::Mat out;
        // in.convertTo(out, -1, beta, 0); //easier way

        // in.convertTo(out, CV_32S); // converts the matrix to type CV_32S to allow aritmetic
        int kappa = 259; // contrast parameter, the higher it is, the lower the effect of contrast
        // double contrast_factor = (kappa * (beta + bgr_max)) / (bgr_max * (kappa - beta)); // calculates contrast factor
        // qDebug() << "Constrast factor"  <<contrast_factor;

        // // contrast is calculated using the equation:
        // // new_pixel = contrast_factor * (old_pixel - 128) + 128
        // cv::Scalar scale(bgr_half, bgr_half, bgr_half); // contains cv scalar depth 3 with values of 128

        // out -= scale; // old_pixel - 128
        // out *= contrast_factor; // contrast * p
        // out += scale; // p + 128
        double factor = 1.0;
        if(beta < 0)
        {
            factor = 1-double(abs(beta)/100.0);
            qDebug() << "COnstarsr factor" << factor << abs(beta);

        }else if(beta > 0)
        {
            factor = 4.0*(beta/100.0)+1;
        }

        in.convertTo(out, -1, factor, 1);
        return out;
    } else
        return in;
}

cv::Mat PreprocessImage::hue(cv::Mat matrix, int h_shift)
{
    qDebug() << "Adjust HUE" << h_shift;
    cv::Mat processed_mat; // initializes output matrix
    cv::cvtColor(matrix, processed_mat, cv::COLOR_BGR2HSV); // converts input matrix to HSV type

    short idx = 0; // index of hue in HSV format
    // iterates through each pixel in mat to ahjust hue values
    for (int y = 0; y < processed_mat.rows; y++) // iterate columns
    {
        for (int x = 0; x < processed_mat.cols; x++) // iterate rows
        {
            short h = processed_mat.at<cv::Vec3b>(y,x)[idx]; // get current hue value at pixel (y, x)
            processed_mat.at<cv::Vec3b>(y,x)[idx] = (h + h_shift) % 180; // adjust hue
        }
    }

    cv::cvtColor(processed_mat, processed_mat, cv::COLOR_HSV2BGR); // converts HSV back to BGR
    return processed_mat;
}
// Gamma
/**
 * Returns an input BGR Mat with gamma adjustments
 *
 * @param matrix The input Mat
 * @param gamma Gamma factor, -100 for low gamma (bright), +100 for high gamma (dark)
 * @return Mat with the gamma adjustments
 */
cv::Mat PreprocessImage::gamma(cv::Mat matrix, double gamma)
{
    // Handles input to be within desired range
    gamma *= 0.05;
    if (gamma < 0)
        gamma = -1 / (gamma - 1);
    else
        gamma += 1;

    cv::Mat processed_mat = matrix.clone(); // initializes output matrix

    short max_n = bgr_max + 1; // number of possible pixel values
    cv::Mat lookUpTable(1, max_n, CV_8U); // lookup table mat
    uchar* p = lookUpTable.ptr(); // pointers for each entry in lookuptable
    for( int i = 0; i < max_n; ++i) // goes through each num in possible range to create lookup value of gamma
    {
        p[i] = cv::saturate_cast<uchar>(std::pow(i / (double)bgr_max, gamma) * (double)bgr_max); // gamma calculation
    }

    cv::LUT(processed_mat, lookUpTable, processed_mat); // uses lookup table to change

    return processed_mat;
}

// Sharpness
/**
 * Returns a sharpened input BGR Mat
 *
 * @param matrix The input Mat
 * @param beta Sharpness factor. Ranges from 0 to 100 (increasing in sharpness)
 * @return Mat with the sharpness adjustments
 */
cv::Mat PreprocessImage::sharpness(cv::Mat matrix, double beta)
{
    // Truncates the input to be within range
    if (beta < 0)
        beta = 0;
    else if (beta > 100)
        beta = -10;
    else
        beta /= -10;

    cv::Mat processed_mat = matrix.clone(); // initializes output matrix

    double sigma = 3; // standard deviation for the gaussian blur
    int size = 3; // kernel size

    double alpha = 1 + -1 * beta; // weight of the original matrix (beta is weight of gaussian blur)
    double gamma = 0; // constant added to the resulting matrix

    cv::GaussianBlur(processed_mat, processed_mat, cv::Size(size, size), sigma, sigma); // creates a matrix adjusted with gaussian blur
    cv::addWeighted(matrix, alpha, processed_mat, beta, gamma, processed_mat); // adds the orignal and blurred matrix with the weights alpha and beta respectively

    return processed_mat;
}

cv::Mat PreprocessImage::manualThreshold(cv::Mat &image,
                                         int threshValue,
                                         uint8_t blurValue)
{
    if (!image.empty()) {
        cv::Mat img = image.clone();
        cvtColor(img, img, cv::COLOR_BGR2GRAY, 1);
        cv::threshold(img, img, threshValue, 255, cv::THRESH_BINARY);
        GaussianBlur(img, img, cv::Size(blurValue, blurValue), 0);
        cvtColor(img, img, cv::COLOR_GRAY2BGR);
        return img;
    } else
        return image;
}


//value from -255 to 255
cv::Mat PreprocessImage::adjustSaturation(cv::Mat &in, int value)
{
    if (!in.empty())
    {
        cv::Mat out;

        cv::cvtColor(in, out, cv::COLOR_BGR2HSV);

        std::vector<cv::Mat> channels;
        cv::split(out, channels); // splits HSV mat into an 3 mats with 1 channel each for H, S and V

        short idx = 1; // index of saturation in HSV format
        short rtype = -1; // use same type as input matrix
        short alpha = 1; // sat_value *= alpha

        channels[idx].convertTo(channels[idx], rtype, alpha, value); // sat_value += s_shift (clips sat_val to stay between 0 to 255)

        cv::merge(channels, out); // merges channels HSV back together into output matrix
        cv::cvtColor(out, out, cv::COLOR_HSV2BGR); // converts HSV back to BGR
        // cv::cvtColor(out, out, cv::COLOR_BGR2BGRA); // converts HSV back to BGR

        return out;
    } else
        return in;

}

void PreprocessImage::hedEdgeDetectDNN(cv::Mat &image,
                                       std::string &prototxt,
                                       std::string &caffemodel,
                                       int size = 128)
{
    cv::dnn::Net net = cv::dnn::readNet(prototxt, caffemodel);

    cv::Mat img;
    cv::cvtColor(image, img, cv::COLOR_BGRA2BGR);
    cv::Size reso(size, size);
    cv::Mat theInput;
    resize(img, theInput, reso);
    cv::Mat blob = cv::dnn::blobFromImage(theInput,
                                          1.0,
                                          reso,
                                          cv::Scalar(104.00698793, 116.66876762, 122.67891434),
                                          false,
                                          false);
    net.setInput(blob);
    cv::Mat out
        = net.forward(); // outputBlobs contains all output blobs for each layer specified in outBlobNames.

    std::vector<cv::Mat> vectorOfImagesFromBlob;
    cv::dnn::imagesFromBlob(out, vectorOfImagesFromBlob);
    cv::Mat tmpMat = vectorOfImagesFromBlob[0] * 255;
    cv::Mat tmpMatUchar;
    tmpMat.convertTo(tmpMatUchar, CV_8U);

           // old code cv::Mat tmpMat = out.reshape(1, reso.height) ;
    cv::resize(tmpMatUchar, image, img.size());
}

void PreprocessImage::CalcBlockMeanVariance(cv::Mat& Img, cv::Mat& Res,float blockSide) // blockSide - the parameter (set greater for larger font on image)
{
    using namespace std;
    using namespace cv;

    Mat I;
    Img.convertTo(I,CV_32FC1);
    Res=Mat::zeros(Img.rows/blockSide,Img.cols/blockSide,CV_32FC1);
    Mat inpaintmask;
    Mat patch;
    Mat smallImg;
    Scalar m,s;

    for(int i=0;i<Img.rows-blockSide;i+=blockSide)
    {
        for (int j=0;j<Img.cols-blockSide;j+=blockSide)
        {
            patch=I(Range(i,i+blockSide+1),Range(j,j+blockSide+1));
            cv::meanStdDev(patch,m,s);
            if(s[0]>0.01) // Thresholding parameter (set smaller for lower contrast image)
            {
                Res.at<float>(i/blockSide,j/blockSide)=m[0];
            }else
            {
                Res.at<float>(i/blockSide,j/blockSide)=0;
            }
        }
    }

    cv::resize(I,smallImg,Res.size());

    cv::threshold(Res,inpaintmask,0.02,1.0,cv::THRESH_BINARY);

    Mat inpainted;
    smallImg.convertTo(smallImg,CV_8UC1,255);

    inpaintmask.convertTo(inpaintmask,CV_8UC1);
    cv::inpaint(smallImg, inpaintmask, inpainted, 5, cv::INPAINT_TELEA);

    cv::resize(inpainted,Res,Img.size());
    Res.convertTo(Res,CV_32FC1,1.0/255.0);

}
