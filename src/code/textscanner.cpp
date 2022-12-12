#include "textscanner.h"
#include <ocs.h>

TextScanner::TextScanner(QObject *parent)
    : QObject{parent}
    ,m_ocr(new OCS(this))
{

}

void TextScanner::setUrl(const QString &url)
{
    m_url = url;
    m_ocr->setFilePath(m_url);
}

bool TextScanner::containsText(const QString &query)
{
    if(m_url.isEmpty())
        return false;

    QString text = m_ocr->getText();

    return text.contains(query, Qt::CaseInsensitive);
}

QString TextScanner::getText()
{
    if(m_url.isEmpty())
        return QString();

    return m_ocr->getText();

}
