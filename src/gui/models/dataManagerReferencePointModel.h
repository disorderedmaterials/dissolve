// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/referencepoint.h"
#include "main/dissolve.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <optional>
#include <vector>

class DataManagerReferencePointModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<ReferencePoint> &referencePoints_;
    Dissolve &dissolve_;

    public:
    DataManagerReferencePointModel(Dissolve &dissolve, std::vector<ReferencePoint> &referencePoints);

    std::optional<std::string> addFile(std::string &&suffix, std::string &&path);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Register all changes to the model
    void update();
};
