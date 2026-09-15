// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "enumOptionsModel.h"
#include "gui2/models/graphModel.h"
#include "nodes/registry.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <QString>
#include <tuple>
#include <vector>

class NodeRegistryModel : public QAbstractListModel
{
    /*
     * NodeRegistryDisplayElement is a type alias for a tuple-like container holing the following
     * information fields about each registered node:
     *
     * - Name/Node::type (QString)
     * - Description/Node::summary (QString)
     * - Category (enum) - currently unavailable
     * - Icon path (QUrl) - currently unavailable
     * - tally (int)
     *
     */
    using NodeRegistryDisplayElement = std::tuple<QString, QString, int>;

    Q_OBJECT

    public:
    explicit NodeRegistryModel(QObject *parent = nullptr);
    ~NodeRegistryModel() override = default;

    enum NodeDisplayRoles
    {
        Name = Qt::DisplayRole,
        Description = Qt::UserRole + 1,
    };
    Q_ENUM(NodeDisplayRoles);

    private:
    // Increment node type
    void increment(QString nodeType);
    // Decrement node type
    void decrement(QString nodeType);
    // Return tally for node type
    int &tally(QString nodeType);
    // Source node registry data
    static std::vector<NodeRegistryDisplayElement> entries_;
    GraphModel *graphModel_{nullptr};

    public:
    // Return a unique default node name for a given node type
    Q_INVOKABLE QString uniqueNodeName(QVariant type);
    // Return node names by category
    Q_INVOKABLE QList<QVariantMap> nodeNames(QString category);
    // Return the enum options for the node categories
    Q_INVOKABLE EnumOptionsModel *categories();
    // Instantiate node from registry
    Q_INVOKABLE void instantiateNode(int x, int y, QVariant type);
    // Set the graph model
    Q_INVOKABLE void setGraphModel(GraphModel *graphModel);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
