// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "procedure/procedure.h"
#include <QIcon>

/*
 * QAbstractItemModel overrides
 */
ProcedureModel::ProcedureModel(Procedure &procedure) : procedure_(procedure) {}

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    // If the index doesn't have a valid internal pointer we're probing the root of the model, so return the number of root
    // sequence nodes
    if (!parent.internalPointer())
        return procedure_.rootSequence().nNodes();

    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    if (node && node->branch())
        return node->branch()->nNodes();

    return 0;
}

int ProcedureModel::columnCount(const QModelIndex &parent) const { return 2; }

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    // Cast the index internal pointer to a node
    auto node = static_cast<ProcedureNode *>(index.internalPointer());
    if (!node)
        return {};

    if (index.column() == 0)
        switch (role)
        {
            case (Qt::DisplayRole):
                if (node->name().empty())
                    return QString::fromStdString(std::string(node->nodeTypes().keyword(node->type())));
                else
                    return QString("%1 (%2)").arg(
                        QString::fromStdString(std::string(node->name())),
                        QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->type()))));
            case (Qt::UserRole):
                return QVariant::fromValue(node->shared_from_this());
            case (Qt::DecorationRole):
                return QIcon((QPixmap(
                    QString(":/nodes/icons/nodes_%1.svg")
                        .arg(
                            QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->type()))).toLower()))));
            default:
                return {};
        }
    else if (index.column() == 1)
        switch (role)
        {
            case (Qt::DisplayRole):
                if (node->scope()->owner())
                    return QString("%1 (%2 branch in %3)")
                        .arg(QString::fromStdString(std::string(ProcedureNode::nodeContexts().keyword(node->scopeContext()))),
                             QString::fromStdString(std::string(node->scope()->blockKeyword())),
                             QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->parent()->type()))));
                else
                    return QString("%1 (in root sequence)")
                        .arg(QString::fromStdString(std::string(ProcedureNode::nodeContexts().keyword(node->scopeContext()))));
            case (Qt::UserRole):
                return QVariant::fromValue(node->shared_from_this());
            default:
                return {};
        }

    return {};
}

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (section == 0)
        return "Node";
    else if (section == 1)
        return "Context";

    return {};
}

QModelIndex ProcedureModel::index(int row, int column, const QModelIndex &parent) const
{
    // Check the parent's internal pointer - if null then it's a root sequence node
    if (!parent.internalPointer())
        return createIndex(row, column, procedure_.rootSequence().sequence()[row].get());

    // Parent is another node, so it should have a branch/sequence that we can refer to
    auto parentNode = static_cast<ProcedureNode *>(parent.internalPointer());
    if (!parentNode || !parentNode->hasBranch())
        return {};

    return createIndex(row, column, parentNode->branch()->sequence()[row].get());
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    auto node = static_cast<ProcedureNode *>(index.internalPointer());
    if (!node)
        return {};

    // If there is no parent then we are an item in the root of the model (i.e. in the root sequence which has no owner)
    auto nodeParent = node->parent();
    if (!nodeParent)
        return createIndex(-1, -1, nullptr);

    // Otherwise we need to find the row index of the parent node in its sequence (i.e. its owner's branch)
    // If the parent's parent is null then we retrieve a row index from the root procedure
    auto nodeParentParent = nodeParent->parent();
    if (!nodeParentParent)
    {
        auto it =
            std::find(procedure_.rootSequence().sequence().begin(), procedure_.rootSequence().sequence().end(), nodeParent);
        return createIndex(it - procedure_.rootSequence().sequence().begin(), 0, nodeParent.get());
    }
    else
    {
        auto children = nodeParentParent->children();
        auto it = std::find(children.begin(), children.end(), nodeParent);
        return createIndex(it - children.begin(), 0, nodeParent.get());
    }
}

bool ProcedureModel::hasChildren(const QModelIndex &parent) const
{
    // If the internal pointer is null then we're questioning the root node, so return 'true'
    if (!parent.internalPointer())
        return true;

    // Check the node for a branch
    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    return (node && node->hasBranch());
}

Qt::ItemFlags ProcedureModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }
