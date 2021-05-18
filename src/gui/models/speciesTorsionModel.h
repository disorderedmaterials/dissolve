// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciestorsion.h"
#include "main/dissolve.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

Q_DECLARE_METATYPE(SpeciesTorsion *)

class SpeciesTorsionModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<SpeciesTorsion> &torsions_;
    Dissolve &dissolve_;

    public:
    SpeciesTorsionModel(std::vector<SpeciesTorsion> &torsions, Dissolve &dissolve);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
