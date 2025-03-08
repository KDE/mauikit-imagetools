#include "ocs.h"
#include <QImage>
#include <QDebug>
#include <QtConcurrent>
#include <QFutureWatcher>

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
    ,m_boxesTypes(BoxType::Word | BoxType::Line | BoxType::Paragraph)
    ,m_confidenceThreshold(50)
{
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

bool OCS::autoRead() const
{
    return m_autoRead;
}

void OCS::setAutoRead(bool value)
{
    if(m_autoRead == value)
        return;
    
    m_autoRead = value;
    Q_EMIT autoReadChanged();
}

void OCS::setBoxesType(OCS::BoxesType types)
{
    if(m_boxesTypes == types)
        return;


    m_boxesTypes = types;
    qDebug() << "Setting the boxes types" << m_boxesTypes << types;

    Q_EMIT boxesTypeChanged();
}

void OCS::setConfidenceThreshold(float value)
{
    if(m_confidenceThreshold == value)
        return;

    m_confidenceThreshold = value;
    Q_EMIT confidenceThresholdChanged();
}

void OCS::getTextAsync()
{
    if(!QUrl::fromUserInput(m_filePath).isLocalFile())
    {
        qDebug() << "URL is not local :: OCR";
        return;
    }
    typedef QMap<BoxType, TextBoxes> Res;
    auto func = [ocs = this](QUrl url, BoxesType levels) -> Res
    {      
        Pix *image = pixRead(url.toLocalFile().toStdString().c_str());
        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        api->Init(NULL, "eng");
        api->SetImage(image);
        api->Recognize(0);

        TextBoxes wordBoxes, lineBoxes, paragraphBoxes;

        auto levelFunc = [ocs](tesseract::TessBaseAPI *api, tesseract::PageIteratorLevel level) -> TextBoxes
        {
            TextBoxes res;
            tesseract::ResultIterator* ri = api->GetIterator();
            if (ri != 0)
            {
                qDebug() << "Getting text for level" << level;
                do
                {
                    const char* word = ri->GetUTF8Text(level);
                    float conf = ri->Confidence(level);
                    int x1, y1, x2, y2;
                    ri->BoundingBox(level, &x1, &y1, &x2, &y2);

                    printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
                           word, conf, x1, y1, x2, y2);

                    if(conf > ocs->m_confidenceThreshold)
                        res << QVariantMap{{"text", QString::fromStdString(word)}, {"rect", QRect{x1, y1, x2-x1, y2-y1}}};
                    delete[] word;
                } while (ri->Next(level));
            }

            return res;
        };

        if(levels.testFlag(Word))
            wordBoxes = levelFunc(api, tesseract::RIL_WORD);

        if(levels.testFlag(Line))
            lineBoxes = levelFunc(api,  tesseract::RIL_TEXTLINE);

        if(levels.testFlag(Paragraph))
            paragraphBoxes = levelFunc(api,  tesseract::RIL_PARA);


        delete api;
        return Res{{Word, wordBoxes}, {Line, lineBoxes}, {Paragraph, paragraphBoxes}};
    };
    
    auto watcher = new QFutureWatcher<Res>;
    connect(watcher, &QFutureWatcher<Res>::finished, [this, watcher]()
            {
              // Q_EMIT textReady(watcher.future().result());
                m_wordBoxes = watcher->result()[Word];
                m_lineBoxes = watcher->result()[Line];
                m_paragraphBoxes = watcher->result()[Paragraph];
                Q_EMIT wordBoxesChanged();
                Q_EMIT lineBoxesChanged();
                Q_EMIT paragraphBoxesChanged();
                watcher->deleteLater();
            });
    
    qDebug() << "GEtting text for boxes " << m_boxesTypes << m_boxesTypes.testFlag(Word);
    QFuture<Res> future = QtConcurrent::run(func, QUrl::fromUserInput(m_filePath), m_boxesTypes);
    watcher->setFuture(future);
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

    if(!m_area.isEmpty())
    {
        QImage img(url.toLocalFile());
        img = img.copy(m_area);
        //    img = img.convertToFormat(QImage::Format_Grayscale8);

        m_tesseract->SetImage(img.bits(), img.width(), img.height(), 4, img.bytesPerLine());

    }else
    {
        Pix* im = pixRead(url.toLocalFile().toStdString().c_str());
        m_tesseract->SetImage(im);
    }

    outText = QString::fromStdString(m_tesseract->GetUTF8Text());

    return outText;
}

void OCS::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    Q_EMIT filePathChanged(m_filePath);
}

void OCS::setArea(QRect area)
{
    if (m_area == area)
        return;

    m_area = area;
    Q_EMIT areaChanged(m_area);
}


TextBoxes OCS::wordBoxes() const
{
    return m_wordBoxes;
}

TextBoxes OCS::paragraphBoxes() const
{
    return m_paragraphBoxes;
}

TextBoxes OCS::lineBoxes() const
{
    return m_lineBoxes;
}

OCS::BoxesType OCS::boxesType()
{
    return m_boxesTypes;
}

float OCS::confidenceThreshold()
{
    return m_confidenceThreshold;
}

void OCS::classBegin()
{
}

void OCS::componentComplete()
{
    qDebug() << "OCS CALSS COMPLETED IN QML";
    connect(this, &OCS::filePathChanged, [this](QString)
            {
                if(m_autoRead)
                {
                    getTextAsync();
                }
            });
    getTextAsync();
}
