// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/dataManagerReferencePointModel.h"
#include "templates/variantpointer.h"
#include <QFileDialog>

DataManagerReferencePointModel::DataManagerReferencePointModel(Dissolve &dissolve, std::vector<ReferencePoint> &referencePoints)
    : referencePoints_(referencePoints), dissolve_(dissolve)
{
}

int DataManagerReferencePointModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return referencePoints_.size();
}

int DataManagerReferencePointModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant DataManagerReferencePointModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto &item = referencePoints_[index.row()];

    switch (role)
    {
        case Qt::DisplayRole:
            return (index.column() == 0 ? std::get<0>(item) : std::get<1>(item)).c_str();
        case Qt::UserRole:
            return VariantPointer<ReferencePoint>(&item);
        default:
            return QVariant();
    }
}

QVariant DataManagerReferencePointModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Data Suffix";
        case 1:
            return "Source";
        default:
            return QVariant();
    }
}

void DataManagerReferencePointModel::update()
{
    beginResetModel();
    endResetModel();
}

std::optional<std::string> DataManagerReferencePointModel::addFile(std::string &&suffix, std::string &&path)
{
    if (!dissolve_.loadRestartAsReference(path, suffix))
        return "Couldn't load the reference point data.\n"
               "Which is odd, annoying, and something you should let the developer know about.";

    beginResetModel();
    referencePoints_.emplace_back(suffix, QDir::current().relativeFilePath(QString(path.c_str())).toStdString());
    endResetModel();

    return {};
}
