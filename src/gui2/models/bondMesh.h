// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include <QList>
#include <QQuick3DGeometry>
#include <QVector3D>
#include <qtmetamacros.h>

class BondMesh : public QQuick3DGeometry
{
    Q_OBJECT
    Q_PROPERTY(const Configuration *configuration READ configuration WRITE setConfiguration);
    Q_PROPERTY(QList<QVector3D> positions READ positions NOTIFY newGeometry)
    Q_PROPERTY(QList<QVector4D> colours READ colours NOTIFY newGeometry)
    Q_PROPERTY(QList<uint> indexes READ indexes NOTIFY newGeometry)
    Q_PROPERTY(double scale READ scale WRITE setScale)

    private:
    const Configuration *config_;
    QList<QVector3D> positions_;
    QList<QVector4D> colours_;
    QList<uint> indexes_;
    double scale_;
    void updateMesh();

    public:
    const Configuration *configuration() const;
    void setConfiguration(const Configuration *config);
    double scale() const;
    void setScale(const double scl);

    QList<QVector3D> positions() const;
    QList<uint> indexes() const;
    QList<QVector4D> colours() const;

    Q_SIGNALS:
    void newGeometry();
};
