// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

class ConfigurationModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source Configuration data
    OptionalReferenceWrapper<const std::vector<std::unique_ptr<Configuration>>> configuration_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<Configuration *>> checkedItems_;
    // Return object represented by specified model index
    Configuration *rawData(const QModelIndex &index) const;

    public:
    enum class ConfigurationUserRole
    {
        RawData = Qt::UserRole,
        MoleculesCount,
        AtomsCount
    };
    Q_ENUM(ConfigurationUserRole);

    public:
    // Set source Configuration data
    void setData(const std::vector<std::unique_ptr<Configuration>> &configuration);
    // Set vector containing checked items
    void setCheckStateData(std::vector<Configuration *> &checkedItemsVector);
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
