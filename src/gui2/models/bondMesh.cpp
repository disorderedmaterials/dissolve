// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/bondMesh.h"
#include "nodes/structure/speciesBase.h"

double BondMesh::scale() const { return scale_; }

void BondMesh::setScale(const double scl)
{
    scale_ = scl;
    if (!positions_.empty())
        updatePositions();
}

const Configuration *BondMesh::configuration() const { return config_; }

void BondMesh::setConfiguration(const Configuration *config)
{
    if (config_ != nullptr && config_ == config)
        updatePositions();
    else
    {
        config_ = config;
        buildMesh();
    }
}

void BondMesh::updatePositions(bool emits)
{
    auto center = config_->box().axisLengths();

    for (auto &&[atom, pos] : zip(config_->atoms(), positions_))
    {
        auto trans = atom.r() - center / 2;
        pos = QVector3D(trans.x, trans.y, trans.z) * scale_;
    }
    if (emits)
        Q_EMIT(newGeometry());
}

void BondMesh::buildMesh()
{
    positions_.clear();
    indexes_.clear();
    colours_.clear();

    if (config_ == nullptr)
        return;

    positions_.resize(config_->atoms().size());
    updatePositions(false);

    colours_.reserve(config_->atoms().size());
    for (auto &atom : config_->atoms())
    {
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
    Q_EMIT(newConfig());
}

QList<QVector3D> BondMesh::positions() const { return positions_; }

QList<QVector4D> BondMesh::colours() const { return colours_; }

QList<uint> BondMesh::indexes() const { return indexes_; }
