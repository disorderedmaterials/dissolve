// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/interactionPotential.h"
#include "kernels/potentials/base.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>

#include <vector>

class GlobalPotentialModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source AtomType data
    const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials_;

    public:
    // Set source AtomType data
    GlobalPotentialModel(const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials);
    ~GlobalPotentialModel() = default;
    const ExternalPotential *rawData(const QModelIndex index) const;
    ExternalPotential *rawData(const QModelIndex index);
    // Update the table contents
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
