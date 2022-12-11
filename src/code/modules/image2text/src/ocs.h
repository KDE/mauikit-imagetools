#pragma once

#include <QObject>
#include <QUrl>
#include <QRect>


namespace tesseract
{
class TessBaseAPI;
}

class OCRLanguageModel;

class OCS : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QRect area READ area WRITE setArea NOTIFY areaChanged)

public:
    explicit OCS(QObject *parent = nullptr);
~OCS();

    QString filePath() const;

    QRect area() const;

public slots:
    QString getText();

    void setFilePath(QString filePath);

    void setArea(QRect area);

private:
    tesseract::TessBaseAPI *m_tesseract;
    OCRLanguageModel *m_languages;

    QString m_filePath;

    QRect m_area;

signals:

    void filePathChanged(QString filePath);
    void areaChanged(QRect area);
};
