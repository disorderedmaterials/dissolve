// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesangle.h"
#include <QAbstractTableModel>
#include <QModelIndex>

Q_DECLARE_METATYPE(SpeciesAngle *)

class SpeciesAngleModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<SpeciesAngle> &angles_;

    public:
    SpeciesAngleModel(std::vector<SpeciesAngle> &angles);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
