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

class OCRLanguageModel;

typedef QVector<QVariantMap> TextBoxes;
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

    /**
     * @brief See the Qt documentation on the QQmlParserStatus.
     */
    void classBegin() override;

    /**
     * @brief See the Qt documentation on the QQmlParserStatus.
     */
    void componentComplete() override;

public Q_SLOTS:
    QString getText();
    void getTextAsync();
    
    void setFilePath(QString filePath);
    void setArea(QRect area);
    void setAutoRead(bool value);
    void setBoxesType(OCS::BoxesType types);
    void setConfidenceThreshold(float value);

private:
    tesseract::TessBaseAPI *m_tesseract;
    OCRLanguageModel *m_languages;

    QString m_filePath;
    QRect m_area;
    bool m_autoRead = false;

    TextBoxes m_wordBoxes;
    TextBoxes m_paragraphBoxes;
    TextBoxes m_lineBoxes;

    BoxesType m_boxesTypes;
    float m_confidenceThreshold;

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
};
Q_DECLARE_OPERATORS_FOR_FLAGS(OCS::BoxesType)
