// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/nodeGraph/nodeRegistryModel.h"
#include "nodes/dissolve.h"

NodeRegistryModel::NodeRegistryModel(QObject *parent)
{
    if (!entries_.empty())
        return;

    // Force instantiation of registry
    if (!NodeRegistry::hasNodeType("Graph"))
        return;

    // Retrieve available node names and summaries from registry
    auto registry = NodeRegistry::producers();
    auto dummyGraph = std::make_unique<DissolveGraph>();

    for (const auto &node : registry)
    {
        auto &[name, producer] = node;
        auto dummyNode = NodeRegistry::produce(dummyGraph->parentGraph(), name);
        entries_.push_back({QString::fromStdString(std::string(dummyNode->type())),
                            QString::fromStdString(std::string(dummyNode->summary()))});
    }
}

// Source node registry data
std::vector<NodeRegistryModel::NodeRegistryDisplayElement> NodeRegistryModel::entries_;

// Instantiate node from registry
void NodeRegistryModel::instantiateNode(int x, int y, QVariant type, QVariant name)
{
    graphModel_->emplace_back(x, y, type, name);
}

// Set the graph model
void NodeRegistryModel::setGraphModel(GraphModel *graphModel) { graphModel_ = graphModel; }

/*
 * QAbstractItemModel overrides
 */

int NodeRegistryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return entries_.size();
}

QVariant NodeRegistryModel::data(const QModelIndex &index, int role) const
{
    auto entry = entries_[index.row()];
    switch (role)
    {
        case NodeDisplayRoles::Name:
            return std::get<0>(entry);
        case NodeDisplayRoles::Description:
            return std::get<1>(entry);
        default:
            return QVariant();
    }
}

Qt::ItemFlags NodeRegistryModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant NodeRegistryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            case 1:
                return "Description";
        }

    return {};
}

QHash<int, QByteArray> NodeRegistryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Name] = "name";
    roles[Description] = "description";
    return roles;
}