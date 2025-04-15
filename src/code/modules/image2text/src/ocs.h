#pragma once

#include <QObject>
#include <QUrl>
#include <QRect>
#include <QVariantMap>
#include "image2text_export.h"
#include <QQmlParserStatus>

namespace tesseract
{
class TessBaseAPI;
}

namespace cv
{
class Mat;
}

typedef QVector<QVariantMap> TextBoxes;

class OCRLanguageModel;
class QImage;

class IMAGE2TEXT_EXPORT OCS : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QRect area READ area WRITE setArea NOTIFY areaChanged)
    
    /** Immediately scan the image after a file url has been provided
     */
    Q_PROPERTY(bool autoRead READ autoRead WRITE setAutoRead NOTIFY autoReadChanged)

    Q_PROPERTY(TextBoxes wordBoxes READ wordBoxes NOTIFY wordBoxesChanged)
    Q_PROPERTY(TextBoxes paragraphBoxes READ paragraphBoxes NOTIFY paragraphBoxesChanged)
    Q_PROPERTY(TextBoxes lineBoxes READ lineBoxes NOTIFY lineBoxesChanged)

    Q_PROPERTY(BoxesType boxesType READ boxesType WRITE setBoxesType NOTIFY boxesTypeChanged)

    /**
     * The confidence thresshold to accept the retrieved text. Only result confidence levels above the threshold will be accepted
     */
    Q_PROPERTY(float confidenceThreshold READ confidenceThreshold WRITE setConfidenceThreshold NOTIFY confidenceThresholdChanged)
    Q_PROPERTY(QString whiteList READ whiteList WRITE setWhiteList NOTIFY whiteListChanged)
    Q_PROPERTY(QString blackList READ blackList WRITE setBlackList NOTIFY blackListChanged FINAL)

    Q_PROPERTY(bool preprocessImage READ preprocessImage WRITE setPreprocessImage NOTIFY preprocessImageChanged FINAL)
    Q_PROPERTY(PageSegMode pageSegMode READ pageSegMode WRITE setPageSegMode NOTIFY pageSegModeChanged)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    
public:

    enum BoxType
    {
        Nothing = 0x0,
        Word = 0x1,
        Paragraph = 0x2,
        Line = 0x4
    };
    Q_DECLARE_FLAGS(BoxesType, BoxType)
    Q_FLAG(BoxesType)
    Q_ENUM(BoxType)

    enum PageSegMode
    {
        Auto,
        Auto_OSD,
        SingleColumn,
        SingleLine,
        SingleBlock,
        SingleWord
    }; Q_ENUM(PageSegMode)

    explicit OCS(QObject *parent = nullptr);
    ~OCS();

    QString filePath() const;
    QRect area() const;
    bool autoRead() const;
    TextBoxes wordBoxes() const;
    TextBoxes paragraphBoxes() const;
    TextBoxes lineBoxes() const;
    OCS::BoxesType boxesType();
    float confidenceThreshold();

    QString whiteList() const;
    QString blackList() const;

    PageSegMode pageSegMode() const;

    bool preprocessImage() const;

    /**
     * @brief See the Qt documentation on the QQmlParserStatus.
     */
    void classBegin() override;

    /**
     * @brief See the Qt documentation on the QQmlParserStatus.
     */
    void componentComplete() override;

    bool ready() const;

public Q_SLOTS:
    QString getText();
    void getTextAsync();
    
    void setFilePath(QString filePath);
    void setArea(QRect area);
    void setAutoRead(bool value);
    void setBoxesType(OCS::BoxesType types);
    void setConfidenceThreshold(float value);

    int wordBoxAt(const QPoint point);
    QVector<int> wordBoxesAt(const QRect &rect);

    void setWhiteList(const QString &value);
    void setBlackList(const QString &value);

    void setPreprocessImage(bool value);
    void setPageSegMode(OCS::PageSegMode value);

    static QString versionString();

private:
    tesseract::TessBaseAPI *m_tesseract;
    OCRLanguageModel *m_languages;

    QString m_filePath;
    QRect m_area;
    bool m_autoRead = false;
    bool m_ready = false;

    TextBoxes m_wordBoxes;
    TextBoxes m_paragraphBoxes;
    TextBoxes m_lineBoxes;

    BoxesType m_boxesTypes;
    float m_confidenceThreshold;

    QString m_whiteList;
    QString m_blackList;

    bool m_preprocessImage = true;

    QMap<QString, PageSegMode> m_segModesModel;
    PageSegMode m_segMode = PageSegMode::Auto_OSD;

    void do_preprocessImage(const QImage &image);

    cv::Mat *m_imgMat = nullptr;///remeber to delete
    QImage *m_ocrImg = nullptr;///remeber to delete

Q_SIGNALS:
    void filePathChanged(QString filePath);
    void areaChanged(QRect area);
    void autoReadChanged();
    void textReady(QString text);
    void wordBoxesChanged();
    void lineBoxesChanged();
    void paragraphBoxesChanged();
    void boxesTypeChanged();
    void confidenceThresholdChanged();
    void whiteListChanged();
    void blackListChanged();
    void preprocessImageChanged();
    void pageSegModeChanged();
    void readyChanged();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(OCS::BoxesType)
