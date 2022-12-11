#include "ocs.h"
#include <QImage>
#include <QDebug>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "OCRLanguageModel.h"
#if TESSERACT_MAJOR_VERSION < 5
#include <tesseract/strngs.h>
#include <tesseract/genericvector.h>
#endif

//static cv::Mat qimageToMatRef(QImage &img, int format)
//    {
//        return cv::Mat(img.height(),
//                       img.width(),
//                       format,
//                       img.bits(),
//                       static_cast<size_t>(img.bytesPerLine()));
//    }
//    static cv::Mat qimageToMat(QImage img, int format)
//    {
//        return cv::Mat(img.height(),
//                       img.width(),
//                       format,
//                       img.bits(),
//                       static_cast<size_t>(img.bytesPerLine()));
//    }
//    static QImage matToQimageRef(cv::Mat &mat, QImage::Format format)
//    {
//        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);
//    }
//    static QImage matToQimage(cv::Mat mat, QImage::Format format)
//    {
//        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);
//    }

OCS::OCS(QObject *parent) : QObject(parent)
  ,m_tesseract(new tesseract::TessBaseAPI())
  ,m_languages(new OCRLanguageModel(this))
{
    if (m_tesseract->Init(nullptr, "eng"))
    {
        qDebug()<<"Failed tesseract OCR init";
        return;
    }
    m_tesseract->SetPageSegMode(tesseract::PSM_AUTO);

    std::vector<std::string> availableLanguages;
#if TESSERACT_MAJOR_VERSION < 5
    GenericVector<STRING> languageVector;
    m_tesseract->GetAvailableLanguagesAsVector(&languageVector);
    for (int i = 0; i < languageVector.size(); i++) {
        availableLanguages.push_back(languageVector[i].c_str());
    }
#else
    m_tesseract->GetAvailableLanguagesAsVector(&availableLanguages);
#endif

    m_languages->setLanguages(availableLanguages);
}

OCS::~OCS()
{
    m_tesseract->End();
}

QString OCS::filePath() const
{
    return m_filePath;
}

QRect OCS::area() const
{
    return m_area;
}

QString OCS::getText()
{
    QUrl url(QUrl::fromUserInput(m_filePath));
    if(!url.isLocalFile())
    {
        qDebug() << "URL is not local :: OCR";
        return "Error!";
    }

    if (m_tesseract->Init(nullptr, m_languages->getLanguagesString().c_str()))
    {
        qDebug() << "Failed tesseract OCR init";
        return "Error!";
    }

        m_tesseract->SetPageSegMode(tesseract::PSM_AUTO);

    QString outText;

//    Pix* im = pixRead(url.toLocalFile().toStdString().c_str());

    QImage img(url.toLocalFile());

    if(!m_area.isEmpty())
    {
           img = img.copy(m_area);
    }
    img = img.convertToFormat(QImage::Format_Grayscale8);

//    qDebug() << img.width << img.height << "LLLLLLLLL";
    m_tesseract->SetImage(img.bits(), img.width(), img.height(), 1, img.bytesPerLine());

    outText = QString::fromStdString(m_tesseract->GetUTF8Text());
//    m_tesseract->End();

    return outText;
}

void OCS::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(m_filePath);
}

void OCS::setArea(QRect area)
{
    if (m_area == area)
        return;

    m_area = area;
    emit areaChanged(m_area);
}


/*!
 * Convert QT QImage to PIX
 * input: QImage
 * result: PIX
 */
static PIX* qImage2PIX(const QImage& qImage) {
    PIX * pixs;

    QImage myImage = qImage.rgbSwapped();
  int width = myImage.width();
  int height = myImage.height();
  int depth = myImage.depth();
  int wpl = myImage.bytesPerLine() / 4;

  pixs = pixCreate(width, height, depth);
  pixSetWpl(pixs, wpl);
  pixSetColormap(pixs, NULL);
  l_uint32 *datas = pixs->data;

  for (int y = 0; y < height; y++) {
    l_uint32 *lines = datas + y * wpl;
    QByteArray a((const char*)myImage.scanLine(y), myImage.bytesPerLine());
    for (int j = 0; j < a.size(); j++) {
      *((l_uint8 *)lines + j) = a[j];
    }
  }

  const qreal toDPM = 1.0 / 0.0254;
  int resolutionX = myImage.dotsPerMeterX() / toDPM;
  int resolutionY = myImage.dotsPerMeterY() / toDPM;

  if (resolutionX < 300) resolutionX = 300;
  if (resolutionY < 300) resolutionY = 300;
  pixSetResolution(pixs, resolutionX, resolutionY);

  return pixEndianByteSwapNew(pixs);
}
