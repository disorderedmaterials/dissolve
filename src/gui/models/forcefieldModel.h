// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <vector>

class ForcefieldModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Forcefield Data
    const std::vector<std::shared_ptr<Forcefield>> &forcefields_;

    public:
    enum ffRoles
    {
        NameRole = Qt::UserRole + 1,
        DescRole,
        RawRole
    };

    /*
     * QAbstractItemModel overrides
     */
    public:
    explicit ForcefieldModel(const std::vector<std::shared_ptr<Forcefield>> &forcefields);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /* Qt::ItemFlags flags(const QModelIndex &index) const override; */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    /* QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override; */
};
