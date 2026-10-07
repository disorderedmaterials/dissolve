// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <QAbstractListModel>
#include <QQuickItem>

class ParameterEndPointsModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    using ParameterEndPoints = std::vector<std::pair<QQuickItem *, QQuickItem *>>;
    ParameterEndPointsModel() = default;

    enum EndPointDisplayRoles
    {
        Source = Qt::DisplayRole,
        Target = Qt::UserRole + 1,
    };

    private:
    // Vector of parameter endpoint QQuickItem * pairs
    ParameterEndPoints endPoints_;

    public:
    // Return vector of parameter endpoint QQuickItem * pairs
    ParameterEndPoints &endPoints();
    const ParameterEndPoints &endPoints() const;
    //
    void removeDuplicates();
    // Add a pair of DropArea QQuickItem *, representing the endpoints
    void add(QQuickItem *sourceDropArea, QQuickItem *targetDropArea);
    // Remove any number of pairs of DropArea QQuickItem *, based on the parent node that has been deleted
    ParameterEndPoints remove(const Node *node);
    // Remove a pair of DropArea QQuickItem *, based on the edge that has been deleted
    ParameterEndPoints remove(const std::string &sourceNode, const std::string &sourceOutput, const std::string &targetNode,
                              const std::string &targetInput);
    // Add all parameter endpoint DropArea pairs from a graph's edges
    void resetFromEdges(const std::vector<std::unique_ptr<Edge>> &edges,
                        const std::map<const Node *, std::map<std::string, QQuickItem *>> &curveOutputsMap,
                        const std::map<const Node *, std::map<std::string, QQuickItem *>> &curveInputsMap);
    // Clear all end points
    void clear();

    private:
    // Remove a pair of DropArea QQuickItem *, based on a lambda determining which pairs are to be deleted
    ParameterEndPoints remove(std::function<bool(int)> lambda);

    public:
    // Replace the target DropArea, for instance when the existing underlying QQuickItem * is no longer valid
    Q_INVOKABLE void replaceTarget(QString nodeName, QString paramName, QQuickItem *newDropArea);
    // Replace the source DropArea, for instance when the existing underlying QQuickItem * is no longer valid
    Q_INVOKABLE void replaceSource(QString nodeName, QString paramName, QQuickItem *newDropArea);

    /*
     * QAbstractListModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
