// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <qtmetamacros.h>

#include <variant>
#include <vector>

class GraphModel;

/** A list model that provides the tick labels of the axis */
class GraphEdgeModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    GraphEdgeModel(GraphModel *parent = nullptr);
    GraphEdgeModel(const GraphEdgeModel &other);

    GraphEdgeModel &operator=(const GraphEdgeModel &other);
    bool operator!=(const GraphEdgeModel &other);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    /** Function to reset the model (and trigger redrawing all labels */

    private:
    GraphModel *parent_;
};

/** A list model that provides the tick labels of the axis */
class GraphNodeModel : public QAbstractListModel
{
    Q_OBJECT

    public:
    GraphNodeModel(GraphModel *parent = nullptr);
    GraphNodeModel(const GraphNodeModel &other);

    GraphNodeModel &operator=(const GraphNodeModel &other);
    bool operator!=(const GraphNodeModel &other);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /** Function to reset the model (and trigger redrawing all labels */

    private:
    GraphModel *parent_;
};

typedef std::variant<double, double *> nodeValue;

double getValue(nodeValue value);

class NodeWrapper
{
    public:
    NodeWrapper(nodeValue value);
    std::string name;
    int posx, posy;
    QVariant value();
    nodeValue &rawValue();

    private:
    nodeValue value_;
};

class GraphModel : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(GraphNodeModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);

    public:
    GraphModel();
    std::vector<NodeWrapper> items;

    public:
    GraphEdgeModel *edges();
    GraphNodeModel *nodes();
    std::string typeName(nodeValue &);
    std::string typeIcon(nodeValue &);

    private:
    GraphNodeModel nodes_;
    GraphEdgeModel edges_;

    Q_SIGNALS:
    void graphChanged();
};
