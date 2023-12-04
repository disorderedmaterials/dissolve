// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/speciesAngle.h"
#include "gui/models/masterTermModel.h"
#include "templates/optionalRef.h"

// MasterAngle model
class MasterAngleModel : public MasterTermModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    public:
    MasterAngleModel(QObject *parent = nullptr);

    private:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<MasterAngle>>> sourceData_;

    public:
    // Set source data
    void setSourceData(std::vector<std::shared_ptr<MasterAngle>> &bonds);
    // Refresh model data
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant getTermData(int row, MasterTermModelData::DataType dataType) const override;
    bool setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value) override;
    
    signals:
    void modelsUpdated();
};
