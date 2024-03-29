#pragma once

#include <QObject>
#include "imagetools_export.h"

class OCS;

/**
 * @brief Text scanner in images.
 */
class IMAGETOOLS_EXPORT TextScanner : public QObject
{
    Q_OBJECT
public:
    explicit TextScanner(QObject *parent = nullptr);
    void setUrl(const QString &url);
    bool containsText(const QString &query);
    QString getText();
private:
    OCS *m_ocr;

    QString m_url;
};
