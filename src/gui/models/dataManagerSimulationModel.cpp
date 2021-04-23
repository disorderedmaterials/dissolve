// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/dataManagerSimulationModel.h"
#include "genericitems/list.h"
#include "templates/variantpointer.h"
#include <QFileDialog>

DataManagerSimulationModel::DataManagerSimulationModel(Dissolve &dissolve, GenericList &items)
    : dissolve_(dissolve), items_(items)
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
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    if (index.row() > items_.items().size() || index.row() < 0)
        return QVariant();

    int idx = 0;
    for (auto &[key, value] : items_.items())
        if (idx++ == index.row())
            switch (index.column())
            {
                case 0:
                    return key.c_str();
                case 1:
                    return std::string(std::get<GenericItem::ClassName>(value)).c_str();
                case 2:
                    return std::get<GenericItem::Version>(value);
                default:
                    return QVariant();
            }
    return QVariant();
}

QVariant DataManagerSimulationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Item Name";
        case 1:
            return "Class";
        case 2:
            return "Version";
        default:
            return QVariant();
    }
}

void DataManagerSimulationModel::update()
{
    beginResetModel();
    endResetModel();
}
