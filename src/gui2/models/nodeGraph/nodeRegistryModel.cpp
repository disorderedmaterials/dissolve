// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/nodeGraph/nodeRegistryModel.h"

NodeRegistryModel::NodeRegistryModel(QObject *parent)
{
    if (elements_.empty())
        populate();
}

// Returns bool - true if node list has been populated
bool NodeRegistryModel::populated() const { return !elements_.empty(); }

// Populate list
void NodeRegistryModel::populate()
{
    // Retrieve available node names and summaries from registry
    auto registry = NodeRegistry::producers();
    for (const auto &node : registry)
    {
        auto &[name, producer] = node;
        auto tempInstantiatedNode = NodeRegistry::produce(name);
        elements_.emplace_back({tempInstantiatedNode->type(), tempInstantiatedNode->summary()});
    }
}

// Instantiate node from registry
void NodeRegistryModel::instantiateNode(GraphModel *graphModel, QVariant type, QVariant name)
{
    graphModel->addNode(std::move(NodeRegistry::produce(graphModel_->graph(), type.toString())), name.toString());
}

/*
 * QAbstractItemModel overrides
 */

int NodeRegistryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return enumOptions_ ? enumOptions_->nOptions() : 0;
}

QVariant NodeRegistryModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || index.column() != 0)
        return {};

    return QString::fromStdString(enumOptions_->keywordByIndex(index.row()));
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

    if (section == 0)
        return "Option";

    return {};
}
