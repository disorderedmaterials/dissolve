// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/bondMesh.h"

const Configuration *BondMesh::configuration() const { return config_; }

void BondMesh::setConfiguration(const Configuration *config)
{
    config_ = config;
    positions_.clear();
    indexes_.clear();

    positions_.append(QVector3D(0.0, 0.0, 0.0));
    positions_.append(QVector3D(100.0, 0.0, 0.0));
    positions_.append(QVector3D(0.0, 0.0, 100.0));

    indexes_.append(0);
    indexes_.append(1);

    indexes_.append(1);
    indexes_.append(2);

    indexes_.append(2);
    indexes_.append(0);
}

QList<QVector3D> BondMesh::positions() const { return positions_; }

QList<uint> BondMesh::indexes() const { return indexes_; }
