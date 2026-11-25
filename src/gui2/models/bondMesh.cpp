// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/bondMesh.h"
#include "classes/species.h"

double BondMesh::scale() const { return scale_; }

void BondMesh::setScale(const double scl)
{
    scale_ = scl;
    updateMesh();
}

const Configuration *BondMesh::configuration() const { return config_; }

void BondMesh::setConfiguration(const Configuration *config)
{
    config_ = config;
    updateMesh();
}

void BondMesh::updateMesh()
{
    if (config_ == nullptr)
        return;
    positions_.clear();
    indexes_.clear();
    colours_.clear();

    auto center = config_->box().axisLengths();

    for (auto &atom : config_->atoms())
    {
        auto trans = atom.r() - center / 2;
        positions_.append(QVector3D(trans.x, trans.y, trans.z) * scale_);
        if (atom.Z() == Elements::C)
            colours_.append(QVector4D(0.0, 1.0, 0.0, 1.0));
        else
            colours_.append(QVector4D(1.0, 1.0, 1.0, 1.0));
    }

    for (auto &mol : config_->molecules())
        for (auto &bond : mol->species()->bonds())
            for (auto *atom : bond.atoms())
                indexes_.append(mol->atoms()[atom->index()]->index());
    Q_EMIT(newGeometry());
}

QList<QVector3D> BondMesh::positions() const { return positions_; }

QList<QVector4D> BondMesh::colours() const { return colours_; }

QList<uint> BondMesh::indexes() const { return indexes_; }
