// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesImproper.h"
#include "gui/models/masterTermModel.h"
#include "gui/models/modelUpdater.h"
#include "templates/optionalRef.h"

// MasterImproper model
class MasterImproperModel : public MasterTermModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public:
    MasterImproperModel(QObject *parent = nullptr);

    private:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<MasterImproper>>> sourceData_;

    public:
    // Set source data
    void setSourceData(std::vector<std::shared_ptr<MasterImproper>> &bonds);
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant getTermData(int row, MasterTermModelData::DataType dataType) const override;
    bool setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value) override;
    const std::shared_ptr<MasterImproper> &rawData(const QModelIndex &index) const;

    private:
    ModelUpdater modelUpdater;
};
