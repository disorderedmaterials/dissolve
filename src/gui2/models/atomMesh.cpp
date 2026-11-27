// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/atomMesh.h"
#include "nodes/structure/species.h"

double AtomMesh::scale() const { return scale_; }

void AtomMesh::setScale(const double scl)
{
    scale_ = scl;
    if (!positions_.empty())
        updatePositions();
}

const Configuration *AtomMesh::configuration() const { return config_; }

void AtomMesh::setConfiguration(const Configuration *config)
{
    if (config_ != nullptr && config_ == config)
        updatePositions();
    else
    {
        config_ = config;
        buildMesh();
    }
}

void AtomMesh::updatePositions(bool emits)
{
    auto center = config_->box().axisLengths();

    Messenger::banner("Updating Atom positions: {}", positions_.size());
    const double offset = 0.1;
    int pos = 0;
    for (auto atom : config_->atoms())
    {
        auto trans = atom.r() - center / 2;

        for (int xs = 0; xs < 2; ++xs)
            for (int ys = 0; ys < 2; ++ys)
                for (int zs = 0; zs < 2; ++zs)
                    positions_[pos++] = QVector3D(trans.x + offset * (2 * xs - 1), trans.y + offset * (2 * ys - 1),
                                                  trans.z + offset * (2 * zs - 1)) *
                                        scale_;
    }
    if (emits)
        Q_EMIT(newGeometry());
}

void AtomMesh::buildMesh()
{
    positions_.clear();
    indexes_.clear();
    colours_.clear();

    if (config_ == nullptr)
        return;

    std::vector<std::tuple<int, int, int>> faces = {
        {0, 1, 2}, {2, 1, 3}, // -x face
        {4, 6, 5}, {5, 6, 7}, // +x face

        {0, 4, 1}, {1, 4, 5}, // -y face
        {2, 3, 6}, {3, 7, 6}, // +y face

        {0, 2, 4}, {4, 2, 6}, // -z face
        {3, 1, 5}, {3, 5, 7}, // +z face
    };

    Messenger::banner("Building Mesh");
    positions_.resize(config_->atoms().size() * 8);
    updatePositions(false);

    Messenger::banner("Building Atom positions: {}", positions_.size());
    int at = 0; // atom count
    for (auto &atom : config_->atoms())
    {
        for (auto &[i, j, k] : faces)
        {
            indexes_.append(8 * at + i);
            indexes_.append(8 * at + j);
            indexes_.append(8 * at + k);
        }

        if (atom.Z() == Elements::C)
            for (int i = 0; i < 8; ++i)
                colours_.append(QVector4D(0.0, 1.0, 0.0, 1.0));
        else
            for (int i = 0; i < 8; ++i)
                colours_.append(QVector4D(1.0, 1.0, 1.0, 1.0));

        ++at;
    }
    Q_EMIT(newGeometry());
    Q_EMIT(newConfig());
}

QList<QVector3D> AtomMesh::positions() const { return positions_; }

QList<QVector4D> AtomMesh::colours() const { return colours_; }

QList<uint> AtomMesh::indexes() const { return indexes_; }
