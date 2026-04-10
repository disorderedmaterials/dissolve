// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesBond.h"
#include "gui/models/commonTermModel.h"
#include "gui/models/modelUpdater.h"

// CommonBond model
class CommonBondModel : public CommonTermModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public:
    explicit CommonBondModel(Species *species);

    private:
    // Source term data
    std::vector<std::shared_ptr<CommonBond>> &sourceData_;

    public:
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant getTermData(int row, CommonTermModelData::DataType dataType) const override;
    bool setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value) override;
    const std::shared_ptr<CommonBond> &rawData(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    private:
    ModelUpdater modelUpdater;
};
