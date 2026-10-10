// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "graphNodeModel.h"
#include "graphModel.h"
#include "nodes/detectMolecules.h"
#include "nodes/dissolve.h"
#include "nodes/inputs.h"
#include "nodes/iterableGraph.h"
#include "nodes/outputs.h"
#include <qvariant.h>

GraphNodeModel::GraphNodeModel(GraphModel *parent) : parent_(parent) { setConnections(); }
GraphNodeModel::GraphNodeModel(const GraphNodeModel &other) : parent_(other.parent_) { setConnections(); }

GraphNodeModel &GraphNodeModel::operator=(const GraphNodeModel &other)
{
    parent_ = other.parent_;
    return *this;
}

bool GraphNodeModel::operator!=(const GraphNodeModel &other) { return &parent_ != &other.parent_; }

// Reset the model
void GraphNodeModel::reset()
{
    auto graph = parent_->graph_;
    auto &nodes = parent_->wrapped_;
    beginResetModel();

    // Remove node-parameter end points corresponding to the previous node set
    for (const auto &wrappedNode : nodes)
    {
        parent_->curveInputEndPoints_.erase(wrappedNode.node());
        parent_->curveOutputEndPoints_.erase(wrappedNode.node());
    }

    // Clear the nodes
    nodes.clear();

    // Emplace all nodes
    for (auto &node : graph->nodes())
        nodes.emplace_back(node.get());

    endResetModel();
}

/* UNUSED
void GraphNodeModel::updateGraph()
{
    beginResetModel();
    endResetModel();
}
*/

//
std::vector<NodeWrapper *> GraphNodeModel::findAllByRoleTrue(int role)
{
    std::vector<NodeWrapper *> nodes;
    for (int i = 0; i < rowCount(); i++)
    {
        auto matches = qvariant_cast<bool>(data(index(i, 0), role));
        if (matches)
        {
            auto node = &parent_->wrapped_[i];
            nodes.push_back(node);
        }
    }
    return nodes;
}

//
void GraphNodeModel::setConnections()
{
    QObject::connect(parent_, &GraphModel::graphRunComplete, this,
                     [this]()
                     {
                         const auto nNodes = parent_->wrapped_.size();
                         for (int i = 0; i < nNodes; i++)
                         {
                             auto index = this->index(i, 0);
                             Q_EMIT dataChanged(index, index, {Qt::UserRole + VERSION});
                         }
                     });
}

/*
 * QAbstractListModel overrides
 */

// Number of nodes (required by QAbstractListModel)
int GraphNodeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent_->graph())
        return 0;
    return parent_->graph()->nodes().size();
}

// Labels for QML roles (required by QAbstractListModel)
QHash<int, QByteArray> GraphNodeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)NAME] = "name";
    roles[Qt::UserRole + (int)POSX] = "posX";
    roles[Qt::UserRole + (int)POSY] = "posY";
    roles[Qt::UserRole + (int)TYPE] = "type";
    roles[Qt::UserRole + (int)ICON] = "icon";
    roles[Qt::UserRole + (int)INPUTS] = "inputs";
    roles[Qt::UserRole + (int)OUTPUTS] = "outputs";
    roles[Qt::UserRole + (int)OPTIONS] = "options";
    roles[Qt::UserRole + (int)HAS_INNER_GRAPH] = "hasInnerGraph";
    roles[Qt::UserRole + (int)IS_ROOT_NODE] = "isRootNode";
    roles[Qt::UserRole + (int)IS_ITERABLE] = "isIterable";
    roles[Qt::UserRole + (int)HAS_PROXY_PARAMETERS] = "hasProxyParameters";
    roles[Qt::UserRole + (int)HAS_DYNAMIC_OUTPUTS] = "hasDynamicOutputs";
    roles[Qt::UserRole + (int)VERSION] = "version";
    return roles;
}

// Data accessor (required by QAbstractListModel)
QVariant GraphNodeModel::data(const QModelIndex &index, int role) const
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case NAME:
            return QString::fromStdString(std::string(item.node()->name()));
        case POSX:
        {
            // If node belongs to a new graph (not a reconstructed graph) attempt to position inputs, outputs and loopbacks in
            // their default x position
            if (!parent_->nodeReconstructionInProgress())
            {
                auto *nodePtr = item.node();
                auto optInitialX = parent_->nodeXPositionInitialiser()(nodePtr);
                if (optInitialX.has_value())
                    nodePtr->x = *optInitialX;
            }
            return item.node()->x;
        }
        case POSY:
        {
            // If node belongs to a new graph (not a reconstructed graph) attempt to position inputs, outputs and loopbacks in
            // their default y position
            if (!parent_->nodeReconstructionInProgress())
            {
                auto *nodePtr = item.node();
                auto optInitialY = parent_->nodeYPositionInitialiser()(nodePtr);
                if (optInitialY.has_value())
                    nodePtr->y = *optInitialY;
            }
            return item.node()->y;
        }
        case TYPE:
            return QString::fromStdString(std::string(item.node()->type()));
        case ICON:
            return QString::fromStdString(std::format("qrc:/DissolveIconsModule/nodes/{}.svg", item.node()->type()));
        case INPUTS:
            return QVariant::fromValue(item.inputs.get());
        case OUTPUTS:
            return QVariant::fromValue(item.outputs.get());
        case OPTIONS:
            return QVariant::fromValue(item.options.get());
        case HAS_INNER_GRAPH:
            return item.hasInner();
        case IS_ROOT_NODE:
            return dynamic_cast<DissolveGraph *>(item.node()->parentGraph()) != nullptr;
        case HAS_PROXY_PARAMETERS:
            return dynamic_cast<InputsNode *>(item.node()) != nullptr || dynamic_cast<OutputsNode *>(item.node()) != nullptr ||
                   dynamic_cast<Graph *>(item.node()) != nullptr || dynamic_cast<IterableGraph *>(item.node()) != nullptr;
        case HAS_DYNAMIC_OUTPUTS:
            return dynamic_cast<DetectMoleculesNode *>(item.node()) != nullptr;
        case VERSION:
            return item.node()->versionIndex();
    }
    return {};
}

bool GraphNodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = parent_->wrapped_[index.row()];
    switch (role - Qt::UserRole)
    {
        case NAME:
        {
            auto name = value.toString().toStdString();
            item.node()->setName(name);
            Q_EMIT dataChanged(index, index, {role});
            return true;
        }
        case POSX:
            item.node()->x = value.toInt();
            // Q_EMIT updatePosition(index.row());
            Q_EMIT dataChanged(index, index, {role});
            return true;
        case POSY:
            item.node()->y = value.toInt();
            // Q_EMIT updatePosition(index.row());
            Q_EMIT dataChanged(index, index, {role});
            return true;
    }
    return false;
}

/*
// Must call *before* inserting new elements.  The count is the number of elements that will be inserted
void GraphNodeModel::beginInsert(int count)
{
    beginInsertRows({}, parent_->graph()->nodes().size(), parent_->graph()->nodes().size() + count);
}

// Must call *after* inserting new elements
void GraphNodeModel::endInsert() { endInsertRows(); }
*/
