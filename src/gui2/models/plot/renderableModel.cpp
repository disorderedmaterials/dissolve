// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "renderableModel.h"
#include "classes/partialSet.h"
#include "math/histogram1D.h"
#include "templates/doubleKeyedMap.h"

RenderableModel::RenderableModel(const Node::NodeParameterMap &outputs)
{
    for (const auto &[name, parameter] : outputs)
    {
        // Handle Data1D source
        if (parameter->storedDataType() == typeid(Data1D))
            setData(index(rowCount(), 0), QVariant::fromValue<RenderableModel::RenderableData>(
                                              {name, [parameter]() -> const Data1D * { return parameter->getPtr<Data1D>(); }}));

        // Handle Histogram1D source
        if (parameter->storedDataType() == typeid(Histogram1D))
            setData(index(rowCount(), 0), QVariant::fromValue<RenderableModel::RenderableData>(
                                              {name, [parameter]() -> const Data1D *
                                               { return &parameter->getPtr<Histogram1D>()->accumulatedData(); }}));

        // Handle PartialSet source
        if (parameter->storedDataType() == typeid(PartialSet *))
        {
            setData(index(rowCount(), 0), QVariant::fromValue<RenderableModel::RenderableData>(
                                              {name + "/" + "Total", [parameter]() -> const Data1D *
                                               { return &parameter->getPtr<PartialSet>()->total(); }}));

            // Do the same for bound and unbound totals and partials...
        }
    }
}

// Update all datasets
void RenderableModel::update()
{
    for (auto &[_, model] : dataSets_)
        model->reset();
}

// Returns bool - true if there are no renderables associated with this node
bool RenderableModel::empty() { return dataSets_.empty(); }

/*
 * QAbstractListModel overrides
 */

//
int RenderableModel::rowCount(const QModelIndex &parent) const { return dataSets_.size(); }

// Set source 1D data
bool RenderableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto renderable = qvariant_cast<RenderableData>(value);
    auto model = std::make_unique<Plot1DModel>(renderable.name, renderable.lambda);
    dataSets_.emplace_back(renderable.name, std::move(model));
    return true;
}

QVariant RenderableModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();
    if (row >= rowCount())
        return {};

    switch (role - Qt::UserRole)
    {
        case KEY:
            return QString::fromStdString(dataSets_.at(row).first);
        case DATA:
            return QVariant::fromValue<Plot1DModel *>(dataSets_.at(row).second.get());
    }
    return {};
}

Qt::ItemFlags RenderableModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> RenderableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)KEY] = "key";
    roles[Qt::UserRole + (int)DATA] = "renderableData";
    roles[Qt::UserRole + (int)TYPE] = "type";
    return roles;
}
