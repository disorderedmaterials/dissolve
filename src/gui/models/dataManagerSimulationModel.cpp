// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/dataManagerSimulationModel.h"
#include "items/list.h"
#include <QFileDialog>

DataManagerSimulationModel::DataManagerSimulationModel(Dissolve &dissolve, GenericList &items)
    : items_(items), dissolve_(dissolve)
{
}

int DataManagerSimulationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return items_.items().size();
}

int DataManagerSimulationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant DataManagerSimulationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};
    if (role != Qt::DisplayRole)
        return {};
    if (index.row() > items_.items().size() || index.row() < 0)
        return {};

    int idx = 0;
    for (auto &[key, value] : items_.items())
        if (idx++ == index.row())
            switch (index.column())
            {
                case 0:
                    return QString::fromStdString(key);
                case 1:
                    return QString::fromStdString(std::string(std::get<GenericItem::ClassName>(value)));
                case 2:
                    return std::get<GenericItem::Version>(value);
                default:
                    return {};
            }
    return {};
}

QVariant DataManagerSimulationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    switch (section)
    {
        case 0:
            return "Item Name";
        case 1:
            return "Class";
        case 2:
            return "Version";
        default:
            return {};
    }
}

void DataManagerSimulationModel::update()
{
    beginResetModel();
    endResetModel();
}
