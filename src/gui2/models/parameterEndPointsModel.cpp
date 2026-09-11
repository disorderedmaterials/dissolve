// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/parameterEndPointsModel.h"
#include "nodes/edge.h"

// Return vector of parameter endpoint QQuickItem * pairs
ParameterEndPointsModel::ParameterEndPoints &ParameterEndPointsModel::endPoints() { return endPoints_; }
const ParameterEndPointsModel::ParameterEndPoints &ParameterEndPointsModel::endPoints() const { return endPoints_; }

void ParameterEndPointsModel::removeDuplicates()
{
    beginResetModel();
    std::sort(endPoints_.begin(), endPoints_.end());
    endPoints_.erase(std::unique(endPoints_.begin(), endPoints_.end()), endPoints_.end());
    endResetModel();
}

void ParameterEndPointsModel::add(QQuickItem *sourceDropArea, QQuickItem *targetDropArea)
{
    int row = endPoints_.size();
    beginInsertRows(QModelIndex(), row, row);
    endPoints_.push_back({sourceDropArea, targetDropArea});
    endInsertRows();
}

ParameterEndPointsModel::ParameterEndPoints ParameterEndPointsModel::remove(const Node *node)
{
    return remove(
        [&](int i) -> bool
        {
            auto &[sourceDropArea, targetDropArea] = endPoints_[i];
            auto sourceParentNode = sourceDropArea->property("parentNodeBox").value<QObject *>();
            auto sourceParentNodeName = sourceParentNode->property("nodeName").toString().toStdString();
            auto targetParentNode = targetDropArea->property("parentNodeBox").value<QObject *>();
            auto targetParentNodeName = targetParentNode->property("nodeName").toString().toStdString();
            return sourceParentNodeName == node->name() || targetParentNodeName == node->name();
        });
}

ParameterEndPointsModel::ParameterEndPoints ParameterEndPointsModel::remove(const std::string &sourceNode,
                                                                            const std::string &sourceOutput,
                                                                            const std::string &targetNode,
                                                                            const std::string &targetInput)
{
    return remove(
        [&](int i) -> bool
        {
            auto &[sourceDropArea, targetDropArea] = endPoints_[i];
            auto sourceParentNode = sourceDropArea->property("parentNodeBox").value<QObject *>();
            auto sourceParentNodeName = sourceParentNode->property("nodeName").toString().toStdString();
            auto sourceParamName = sourceDropArea->property("paramName").toString().toStdString();
            auto targetParentNode = targetDropArea->property("parentNodeBox").value<QObject *>();
            auto targetParentNodeName = targetParentNode->property("nodeName").toString().toStdString();
            auto targetParamName = targetDropArea->property("paramName").toString().toStdString();
            auto isParentConnection = (sourceParentNodeName == sourceNode && targetParentNodeName == targetNode);
            return isParentConnection && (sourceParamName == sourceOutput && targetParamName == targetInput);
        });
}

// Add all parameter endpoint pairs from a graph's edges
void ParameterEndPointsModel::resetFromEdges(const std::vector<std::unique_ptr<Edge>> &edges,
                                             const std::map<const Node *, std::map<std::string, QQuickItem *>> &curveOutputsMap,
                                             const std::map<const Node *, std::map<std::string, QQuickItem *>> &curveInputsMap)
{
    clear();
    for (const auto &edge : edges)
    {
        const auto *sourceNodePtr = &(edge->sourceNode());
        const auto &output = edge->sourceOutput();
        const auto *targetNodePtr = &(edge->targetNode());
        const auto &input = edge->targetInput();
        auto outputEndPointsMap = curveOutputsMap.find(sourceNodePtr)->second;
        auto inputEndPointsMap = curveInputsMap.find(targetNodePtr)->second;
        add(outputEndPointsMap[std::string(output.name())], inputEndPointsMap[std::string(input.name())]);
    }
}

// Clear all end points
void ParameterEndPointsModel::clear()
{
    beginResetModel();
    endPoints_.clear();
    endResetModel();
}

// Remove a pair of DropArea QQuickItem *, based on a lambda determining which pairs are to be deleted
ParameterEndPointsModel::ParameterEndPoints ParameterEndPointsModel::remove(std::function<bool(int)> lambda)
{
    ParameterEndPoints removed;
    for (int row = endPoints_.size() - 1; row >= 0; --row)
    {
        if (lambda(row))
        {
            removed.emplace_back(endPoints_.at(row));
            beginRemoveRows(QModelIndex(), row, row);
            endPoints_.erase(endPoints_.begin() + row);
            endRemoveRows();
        }
    }
    return removed;
}

// Replace the target DropArea, for instance when the existing underlying QQuickItem * is no longer valid
void ParameterEndPointsModel::replaceTarget(QString nodeName, QString paramName, QQuickItem *newDropArea)
{
    // If no endpoints present, don't do anything
    if (endPoints_.empty())
        return;

    // Find the new DropArea in the input endpoints, inorder to replace the current target endpoint with it
    auto replaceIt = std::find_if(endPoints_.begin(), endPoints_.end(),
                                  [&](const std::pair<QQuickItem *, QQuickItem *> &pair)
                                  {
                                      auto &target = pair.second;
                                      auto targetNodeName = target->property("nodeName").toString();
                                      auto targetParam = target->property("paramName").toString();
                                      return targetNodeName == nodeName && targetParam == paramName;
                                  });

    beginResetModel();
    auto index = std::distance(endPoints_.begin(), replaceIt);
    endPoints_[index].second = newDropArea;
    endResetModel();
}

// Replace the target DropArea, for instance when the existing underlying QQuickItem * is no longer valid
void ParameterEndPointsModel::replaceSource(QString nodeName, QString paramName, QQuickItem *newDropArea)
{
    // If no endpoints present, don't do anything
    if (endPoints_.empty())
        return;

    // Find the new DropArea in the input endpoints, inorder to replace the current target endpoint with it
    auto replaceIt = std::find_if(endPoints_.begin(), endPoints_.end(),
                                  [&](const std::pair<QQuickItem *, QQuickItem *> &pair)
                                  {
                                      auto &source = pair.first;
                                      auto sourceNodeName = source->property("nodeName").toString();
                                      auto sourceParam = source->property("paramName").toString();
                                      return sourceNodeName == nodeName && sourceParam == paramName;
                                  });

    auto index = std::distance(endPoints_.begin(), replaceIt);
    beginResetModel();
    endPoints_[index].first = newDropArea;
    endResetModel();
}

/*
 * QAbstractListModel overrides
 */

int ParameterEndPointsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return endPoints_.size();
}

QVariant ParameterEndPointsModel::data(const QModelIndex &index, int role) const
{
    auto &[source, target] = endPoints_[index.row()];
    switch (role)
    {
        case EndPointDisplayRoles::Source:
            return QVariant::fromValue(source);
        case EndPointDisplayRoles::Target:
            return QVariant::fromValue(target);
        default:
            return QVariant();
    }
}

Qt::ItemFlags ParameterEndPointsModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant ParameterEndPointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "SourceDropArea";
            case 1:
                return "TargetDropArea";
        }

    return {};
}

QHash<int, QByteArray> ParameterEndPointsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Source] = "sourceDropArea";
    roles[Target] = "targetDropArea";
    return roles;
}