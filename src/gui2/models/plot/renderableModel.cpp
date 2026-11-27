// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "renderableModel.h"
#include "classes/partialSet.h"
#include "math/histogram1D.h"
#include "templates/doubleKeyedMap.h"
#include <stdexcept>

RenderableModel::RenderableModel(const Node::NodeParameterMap &outputs)
{
    for (const auto &[name, parameter] : outputs)
    {
        // Handle Data1D source
        if (parameter->storedDataType() == typeid(Data1D))
        {
            ++nExpectedDatasets_;
            setData(index(rowCount(), 0),
                    QVariant::fromValue<RenderableModel::RenderableData>({name, parameter->getPtr<Data1D>()}));
        }

        // Handle Data2D source
        if (parameter->storedDataType() == typeid(Data2D))
        {
            ++nExpectedDatasets_;
            setData(index(rowCount(), 0),
                    QVariant::fromValue<RenderableModel::RenderableData>({name, parameter->getPtr<Data2D>()}));
        }

        // Handle Histogram1D source
        if (parameter->storedDataType() == typeid(Histogram1D))
        {
            ++nExpectedDatasets_;
            setData(index(rowCount(), 0), QVariant::fromValue<RenderableModel::RenderableData>(
                                              {name, &parameter->getPtr<Histogram1D>()->accumulatedData()}));
        }

        // Handle PartialSet source
        if (parameter->storedDataType() == typeid(PartialSet *))
        {
            // TODO: increment for each of total and partials
            ++nExpectedDatasets_;

            const auto *partialSet = parameter->get<PartialSet *>();
            if (!partialSet)
                continue;

            setData(index(rowCount(), 0),
                    QVariant::fromValue<RenderableModel::RenderableData>({name + "/" + "Total", &partialSet->total()}));

            // Do the same for bound and unbound totals and partials...
        }
    }
}

// Update all datasets
void RenderableModel::update()
{
    for (auto &[_, model] : datasets_)
        model->handleDataChanged();
}

// Returns bool - true if there are no renderables associated with this node
bool RenderableModel::empty() { return datasets_.empty(); }

// Returns bool - true if the number of realized datasets matches the number of expected datasets
bool RenderableModel::renderablesRealized() const { return nExpectedDatasets_ == datasets_.size(); }

/*
 * QAbstractListModel overrides
 */

int RenderableModel::rowCount(const QModelIndex &parent) const { return datasets_.size(); }

// Set source 1D data
bool RenderableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const auto renderable = qvariant_cast<RenderableData>(value);
    std::unique_ptr<PlotModelBase> model = nullptr;

    if (std::holds_alternative<const Data1D *>(renderable.data))
        model = std::make_unique<Plot1DModel>(renderable.name, std::get<const Data1D *>(renderable.data));
    else if (std::holds_alternative<const Data2D *>(renderable.data))
        model = std::make_unique<Plot2DModel>(renderable.name, std::get<const Data2D *>(renderable.data));
    else
        return false;

    datasets_.emplace_back(renderable.name, std::move(model));
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
            return QString::fromStdString(datasets_.at(row).first);
        case DATA:
        {
            auto *dataset = datasets_.at(row).second.get();
            switch (dataset->N)
            {
                case 1:
                    return QVariant::fromValue<Plot1DModel *>(static_cast<Plot1DModel *>(dataset));
                case 2:
                    return QVariant::fromValue<Plot2DModel *>(static_cast<Plot2DModel *>(dataset));
                default:
                    throw std::runtime_error("Attempted to load renderable dataset with invalid dimensionalty.");
            }
        }
        case DIMENSIONS:
            return datasets_.at(row).second->N;
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
    roles[Qt::UserRole + (int)DIMENSIONS] = "dimensions";
    return roles;
}
