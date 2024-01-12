// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/layer.h"
#include "templates/optionalRef.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <vector>

class ModuleLayersModel : public QAbstractListModel
{
    Q_OBJECT

    private:
    // Source ModuleLayers data
    OptionalReferenceWrapper<const std::vector<std::unique_ptr<ModuleLayer>>> layers_;
    // Reference to CoreData (for Configuration target setting)
    OptionalReferenceWrapper<CoreData> coreData_;

    public:
    // Set source ModuleLayers data
    void setData(const std::vector<std::unique_ptr<ModuleLayer>> &layers, CoreData &coreData);
    // Return object represented by specified model index
    ModuleLayer *rawData(const QModelIndex &index) const;
    void reset();

    public:
    enum class ModuleLayersUserRole
    {
        RawData = Qt::UserRole,
        ModuleLayerModel
    };
    Q_ENUM(ModuleLayersUserRole);

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
