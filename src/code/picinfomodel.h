// Copyright 2018-2020 Camilo Higuita <milo.h@aol.com>
// Copyright 2018-2020 Nitrux Latinoamericana S.C.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PICINFOMODEL_H
#define PICINFOMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include <MauiKit/Core/mauilist.h>

#include <QFileInfo>

class PicInfoModel : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString fileName MEMBER m_fileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(double lat READ latitude NOTIFY dataReady FINAL)
    Q_PROPERTY(double lon READ longitude NOTIFY dataReady FINAL)

public:
    enum ROLES { KEY, VALUE };
    explicit PicInfoModel(QObject *parent = nullptr);

    QUrl url() const;
    void componentComplete() override final;
    
public slots:
    void setUrl(QUrl url);

    double latitude() const;
    double longitude() const;

private:
    QUrl m_url;
    QString m_fileName;
    FMH::MODEL_LIST m_data;

    double m_longitude;
    double m_latitude;

    void parse();

signals:
    void urlChanged(QUrl url);
    void fileNameChanged();
    void dataReady();

    // MauiList interface
public:
    const FMH::MODEL_LIST &items() const override;
};

#endif // PICINFOMODEL_H
