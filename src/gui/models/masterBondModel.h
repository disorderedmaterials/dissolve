// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesBond.h"
#include "gui/models/masterTermModel.h"
#include "gui/models/modelUpdater.h"
#include "templates/optionalRef.h"

// MasterBond model
class MasterBondModel : public MasterTermModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public:
    explicit MasterBondModel(CoreData &coreData);

    private:
    // Source term data
    std::vector<std::shared_ptr<MasterBond>> &sourceData_;

    public:
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant getTermData(int row, MasterTermModelData::DataType dataType) const override;
    bool setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value) override;
    const std::shared_ptr<MasterBond> &rawData(const QModelIndex &index) const;

    private:
    ModelUpdater modelUpdater;
};
