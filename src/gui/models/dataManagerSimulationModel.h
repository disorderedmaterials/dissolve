// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QString>
#include <optional>
#include <vector>

class DataManagerSimulationModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    GenericList &items_;
    Dissolve &dissolve_;

    public:
    DataManagerSimulationModel(Dissolve &dissolve, GenericList &items);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Register all changes to the model
    void update();

    Q_SIGNALS:
    void closeClicked();
    void simulationDataFilter();
};
