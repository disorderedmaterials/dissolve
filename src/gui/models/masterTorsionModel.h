// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesTorsion.h"
#include "gui/models/masterTermModel.h"
#include "gui/models/modelUpdater.h"
#include "templates/optionalRef.h"

// MasterTorsion model
class MasterTorsionModel : public MasterTermModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public:
    MasterTorsionModel(QObject *parent = nullptr);

    private:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<MasterTorsion>>> sourceData_;

    public:
    // Set source data
    void setSourceData(std::vector<std::shared_ptr<MasterTorsion>> &terms);
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant getTermData(int row, MasterTermModelData::DataType dataType) const override;
    bool setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value) override;
    const std::shared_ptr<MasterTorsion> &rawData(const QModelIndex &index) const;

    private:
    ModelUpdater modelUpdater;
};
