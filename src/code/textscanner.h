#ifndef TEXTSCANNER_H
#define TEXTSCANNER_H

#include <QObject>
#include "imagetools_export.h"

class OCS;
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
signals:

};

#endif // TEXTSCANNER_H
