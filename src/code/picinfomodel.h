// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QObject>

#include <MauiKit4/Core/mauilist.h>

#include <QFileInfo>

class Exiv2Extractor;

/**
 * @brief
 */
class PicInfoModel : public MauiList
{
    Q_OBJECT
    
    /**
     * 
     */
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString fileName MEMBER m_fileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(double lat READ latitude NOTIFY dataReady FINAL)
    Q_PROPERTY(double lon READ longitude NOTIFY dataReady FINAL)
    Q_PROPERTY(double alt READ altitude NOTIFY dataReady FINAL)

public:
    enum ROLES { KEY, VALUE };
    explicit PicInfoModel(QObject *parent = nullptr);

    QUrl url() const;
    void componentComplete() override final;
    
public Q_SLOTS:
    void setUrl(QUrl url);

    double latitude() const;
    double longitude() const;
    double altitude() const;

    bool removeTag(const QString &tag);
    bool editTag(const QString &tag, const QString &value);

    bool setGpsData(const QString &latitude, const QString &longitude, const QString &altitude ="0.0");
    bool setComment(const QString &comment);
    bool removeGpsData();

private:
    Exiv2Extractor *m_extractor;
    QUrl m_url;
    QString m_fileName;
    FMH::MODEL_LIST m_data;

    double m_longitude;
    double m_latitude;
    double m_altitude;

    void parse();


Q_SIGNALS:
    void urlChanged(QUrl url);
    void fileNameChanged();
    void dataReady();

    // MauiList interface
public:
    const FMH::MODEL_LIST &items() const override;
};
