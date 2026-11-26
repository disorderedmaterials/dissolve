// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/atomMesh.h"
#include "classes/species.h"

double AtomMesh::scale() const { return scale_; }

void AtomMesh::setScale(const double scl)
{
    scale_ = scl;
    updateMesh();
}

const Configuration *AtomMesh::configuration() const { return config_; }

void AtomMesh::setConfiguration(const Configuration *config)
{
    config_ = config;
    updateMesh();
}

void AtomMesh::updateMesh()
{
    if (config_ == nullptr)
        return;
    positions_.clear();
    indexes_.clear();
    colours_.clear();

    std::vector<std::tuple<int, int, int>> faces = {
        {0, 1, 2}, {2, 1, 3}, // -x face
        {4, 6, 5}, {5, 6, 7}, // +x face

        {0, 4, 1}, {1, 4, 5}, // -y face
        {2, 3, 6}, {3, 7, 6}, // +y face

        {0, 2, 4}, {4, 2, 6}, // -z face
        {3, 1, 5}, {3, 5, 7}, // +z face
    };

    auto center = config_->box().axisLengths();

    int at = 0; // atom count
    const double offset = 0.1;
    for (auto &atom : config_->atoms())
    {
        auto trans = atom.r() - center / 2;

        for (int xs = 0; xs < 2; ++xs)
            for (int ys = 0; ys < 2; ++ys)
                for (int zs = 0; zs < 2; ++zs)
                    positions_.append(QVector3D(trans.x + offset * (2 * xs - 1), trans.y + offset * (2 * ys - 1),
                                                trans.z + offset * (2 * zs - 1)) *
                                      scale_);

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
}

QList<QVector3D> AtomMesh::positions() const { return positions_; }

QList<QVector4D> AtomMesh::colours() const { return colours_; }

QList<uint> AtomMesh::indexes() const { return indexes_; }
