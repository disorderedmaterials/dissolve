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
     * NodeRegistryDisplayElement is a type alias for a container holding the following
     * information fields about each registered node:
     *
     * - Name/Node::type (QString)
     * - Description/Node::summary (QString)
     * - Category (enum) - currently unavailable
     * - Icon path (QUrl) - currently unavailable
     *
     */
    struct NodeRegistryDisplayElement
    {
        QString name;
        QString description;
    };

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
    // Source node registry data
    static std::vector<NodeRegistryDisplayElement> entries_;
    GraphModel *graphModel_{nullptr};
    // Record of all parameters and their tally
    std::map<std::string, int> allInputs_;
    std::map<std::string, int> allOutputs_;

    public:
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
