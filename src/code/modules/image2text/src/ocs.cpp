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

// #include "preprocessimage.hpp"
#include <preprocessimage.hpp>
#include <convertimage.hpp>
#include <cvmatandqimage.h>

static QMap<QString, QMap<OCS::BoxType, TextBoxes>> OCRCache;

OCS::OCS(QObject *parent) : QObject(parent)
    ,m_tesseract(new tesseract::TessBaseAPI())
    ,m_languages(new OCRLanguageModel(this))
    ,m_boxesTypes(BoxType::Word | BoxType::Line | BoxType::Paragraph)
    ,m_confidenceThreshold(50)
// ,m_whiteList("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz")
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
    qDebug() << "OCS object has been deleted" << this;
    m_tesseract->End();
    delete m_tesseract;
    m_tesseract = nullptr;
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

int OCS::wordBoxAt(const QPoint point)
{
    int i = 0;
    for(const auto &box : m_wordBoxes)
    {
        QRect rect = box["rect"].toRect();

        qDebug() << "Rect: " << rect << "Point: " << point << rect.contains(point, true);

        if(rect.contains(point))
            return i;

        i++;
    }

    return i;
}

QVector<int> OCS::wordBoxesAt(const QRect &rect)
{
    QVector<int> res;
    int i = 0;
    for(const auto &box : m_wordBoxes)
    {
        QRect rect_o = box["rect"].toRect();

        if(rect.intersects(rect_o))
            res << i;

        i++;
    }

    return res;
}

QVector<int> OCS::allWordBoxes()
{
    QVector<int> res;
    int i = 0;
    for(const auto &box : m_wordBoxes)
    {
        res << i;
        i++;
    }

    return res;
}

void OCS::setWhiteList(const QString &value)
{
    if(value == m_whiteList)
        return;

    m_whiteList = value;
    Q_EMIT whiteListChanged();
}

void OCS::setBlackList(const QString &value)
{
    if(value == m_blackList)
        return;

    m_blackList = value;
    Q_EMIT blackListChanged();
}

void OCS::setPreprocessImage(bool value)
{
    if(m_preprocessImage == value)
        return;

    m_preprocessImage = value;

    Q_EMIT preprocessImageChanged();
}

void OCS::setPageSegMode(PageSegMode value)
{
    if(m_segMode == value)
        return;

    m_segMode = value;
    Q_EMIT pageSegModeChanged();
}

QString OCS::versionString()
{
    return QString::fromStdString(tesseract::TessBaseAPI::Version());
}

void OCS::do_preprocessImage(const QImage &image)
{


}

static tesseract::PageSegMode mapPageSegValue(OCS::PageSegMode value)
{
    switch(value)
    {
    default:
    case OCS::PageSegMode::Auto: return tesseract::PageSegMode::PSM_AUTO;
    case OCS::PageSegMode::Auto_OSD: return tesseract::PageSegMode::PSM_AUTO_OSD;
    case OCS::PageSegMode::SingleColumn: return tesseract::PageSegMode::PSM_SINGLE_COLUMN;
    case OCS::PageSegMode::SingleLine: return tesseract::PageSegMode::PSM_SINGLE_LINE;
    case OCS::PageSegMode::SingleBlock: return tesseract::PageSegMode::PSM_SINGLE_BLOCK;
    case OCS::PageSegMode::SingleWord: return tesseract::PageSegMode::PSM_SINGLE_WORD;
    }
}

void OCS::getTextAsync()
{
    m_ready = false;
    Q_EMIT readyChanged();
    if(!QUrl::fromUserInput(m_filePath).isLocalFile())
    {
        qDebug() << "URL is not local :: OCR";
        return;
    }

    typedef QMap<BoxType, TextBoxes> Res;
    auto func = [ocs = this](QUrl url, BoxesType levels) -> Res
    {
        if(ocs->m_tesseract == nullptr)
        {
            return Res{};
        }

        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        api->Init(NULL, "eng");

        api->SetVariable("tessedit_char_whitelist",
                         ocs->m_whiteList.toStdString().c_str());
        api->SetVariable("tessedit_char_blacklist",
                         ocs->m_blackList.toStdString().c_str());

        api->SetPageSegMode(mapPageSegValue(ocs->m_segMode));
        ocs->m_ocrImg = new QImage(url.toLocalFile());

        if(ocs->m_preprocessImage)
        {
            auto m_imgMat = ConvertImage::qimageToMatRef(*ocs->m_ocrImg, CV_8UC4);

                   // PreprocessImage::toGray(m_imgMat,1);
            // PreprocessImage::adaptThreshold(m_imgMat, false, 3, 1);

            auto m_ocrImg = ConvertImage::matToQimageRef(m_imgMat, QImage::Format_RGBA8888); //remember to delete

            api->SetImage(m_ocrImg.bits(), m_ocrImg.width(), m_ocrImg.height(), 4, m_ocrImg.bytesPerLine());

        }else
        {
            api->SetImage(ocs->m_ocrImg->bits(), ocs->m_ocrImg->width(), ocs->m_ocrImg->height(), 4,
                          ocs->m_ocrImg->bytesPerLine());
        }        

        api->SetSourceResolution(200);

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

                    if(conf > ocs->m_confidenceThreshold && !isspace(*word))
                    {
                        bool bold, italic, underlined, monospace, serif, smallcaps = false;
                        int pointsize, fontid;
                        auto prop = ri->WordFontAttributes(&bold, &italic, &underlined, &monospace, &serif, &smallcaps, &pointsize, &fontid);
                        res << QVariantMap{{"text", QString::fromStdString(word)},
                                           {"rect", QRect{x1, y1, x2-x1, y2-y1}},
                                           {"bold", bold}, {"italic", italic},
                                           {"underlined", underlined},
                                           {"monospace", monospace},
                                           {"serif", serif},
                                           {"pointsize", pointsize}};


                    }
                    // qDebug()<< res;

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

        api->End();
        delete api;
        return Res{{Word, wordBoxes}, {Line, lineBoxes}, {Paragraph, paragraphBoxes}};
    };    
    
    qDebug() << "GEtting text for boxes " << m_boxesTypes << m_boxesTypes.testFlag(Word);

    if(OCRCache.contains(m_filePath))
    {
        qDebug() << "OCR retrieved from cached";
        auto res =  OCRCache[m_filePath];
        m_wordBoxes = res[Word];
        m_lineBoxes = res[Line];
        m_paragraphBoxes = res[Paragraph];
        Q_EMIT wordBoxesChanged();
        Q_EMIT lineBoxesChanged();
        Q_EMIT paragraphBoxesChanged();
        m_ready = true;
        Q_EMIT readyChanged();
    }else
    {
        auto watcher = new QFutureWatcher<Res>;
        connect(watcher, &QFutureWatcher<Res>::finished, [this, watcher, url = m_filePath]()
                {

                    Res res;
                    res = watcher->result();

                    if(!this->m_tesseract)
                    {
                        qDebug() << "Results from oCR operation discarded OCS object has been deleted";
                    }else
                    {
                        m_wordBoxes = res[Word];
                        m_lineBoxes = res[Line];
                        m_paragraphBoxes = res[Paragraph];
                        Q_EMIT wordBoxesChanged();
                        Q_EMIT lineBoxesChanged();
                        Q_EMIT paragraphBoxesChanged();
                        m_ready = true;
                        Q_EMIT readyChanged();
                    }

                    OCRCache.insert(url, res);
                    watcher->deleteLater();
                });

        QFuture<Res> future = QtConcurrent::run(func, QUrl::fromUserInput(m_filePath), m_boxesTypes);
        watcher->setFuture(future);

               // connect(this, &OCS::destroyed, [this, watcher]()
               //         {
               //             if(watcher)
               //             {
               //                 qDebug() << "OCS was destroyed before OCR process finished, so cancel it";
               //                 watcher->future().cancel();
               //                 // watcher->deleteLater();
               //             }
               // });
    }
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

QString OCS::whiteList() const
{
    return m_whiteList;
}

QString OCS::blackList() const
{
    return m_blackList;
}

OCS::PageSegMode OCS::pageSegMode() const
{
    return m_segMode;
}

bool OCS::preprocessImage() const
{
    return m_preprocessImage;
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

bool OCS::ready() const
{
    return m_ready;
}
