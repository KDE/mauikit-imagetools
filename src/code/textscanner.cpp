#include "textscanner.h"

#ifndef Q_OS_ANDROID
#include <ocs.h>
#endif

TextScanner::TextScanner(QObject *parent)
    : QObject{parent}
    #ifndef Q_OS_ANDROID
    ,m_ocr(new OCS(this))
    #endif
{

}

void TextScanner::setUrl(const QString &url)
{
#ifndef Q_OS_ANDROID
    m_url = url;
    m_ocr->setFilePath(m_url);
#endif
}

bool TextScanner::containsText(const QString &query)
{
#ifndef Q_OS_ANDROID

    if(m_url.isEmpty())
        return false;

    QString text = m_ocr->getText();

    return text.contains(query, Qt::CaseInsensitive);
#endif

    return false;
}

QString TextScanner::getText()
{
#ifndef Q_OS_ANDROID

    if(m_url.isEmpty())
        return QString();

    return m_ocr->getText();
#endif

    return QString();
}
