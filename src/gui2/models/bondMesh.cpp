// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/bondMesh.h"
#include "classes/species.h"

const Configuration *BondMesh::configuration() const { return config_; }

void BondMesh::setConfiguration(const Configuration *config)
{
    config_ = config;
    if (config_ == nullptr)
        return;
    positions_.clear();
    indexes_.clear();

    for (auto &atom : config->atoms())
        positions_.append(QVector3D(atom.r().x, atom.r().y, atom.r().z) * 100.0);

    for (auto &mol : config->molecules())
        for (auto &bond : mol->species()->bonds())
            for (auto *atom : bond.atoms())
                indexes_.append(mol->atoms()[atom->index()]->index());
    Q_EMIT(newGeometry());
}

QList<QVector3D> BondMesh::positions() const { return positions_; }

QList<uint> BondMesh::indexes() const { return indexes_; }
