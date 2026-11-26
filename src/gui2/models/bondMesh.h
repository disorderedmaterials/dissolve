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
    Q_PROPERTY(QList<QVector4D> colours READ colours NOTIFY newConfig)
    Q_PROPERTY(QList<uint> indexes READ indexes NOTIFY newConfig)
    Q_PROPERTY(double scale READ scale WRITE setScale)

    private:
    // The configuration which holds our bonds
    const Configuration *config_;
    // Mesh vertices as list of x, y, z
    QList<QVector3D> positions_;
    // Vertex colours as RGBA.  Color 0 is black and 1 is white.
    // Alpha 0 is transparent and Alpha 1 is opaque
    QList<QVector4D> colours_;
    // Connections between vertices.  Each non-overlapping pair of
    // indices is a line between those two vertices from the
    // positions_ array
    QList<uint> indexes_;
    // Scaling to magnify the mesh
    double scale_;
    // Create the mesh for a new configuration
    void buildMesh();
    // Update positions in the existing mesh
    void updatePositions(bool emits = true);

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
    void newConfig();
};
