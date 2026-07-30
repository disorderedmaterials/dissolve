// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/nodeGraph/graphModel.h"
#include "nodes/registry.h"
#include <QAbstractListModel>
#include <QModelIndex>
#include <tuple>
#include <vector>

class NodeRegistryModel : public QAbstractListModel
{
    /*
     * NodeRegistryDisplayElement is a type alias for a tuple-like container holing the following
     * information fields about each registered node:
     *
     * - Name/Node::type (string)
     * - Description/Node::summary (string)
     * - Category (enum) - currently unavailable
     *
     */
    using NodeRegistryDisplayElement = std::tuple<std::string_view, std::string_view>;

    Q_OBJECT

    public:
    explicit NodeRegistryModel(QObject *parent = nullptr);
    ~NodeRegistryModel() override = default;

    private:
    // Source node registry data
    static std::vector<const NodeRegistryDisplayElement> elements_;

    public:
    // Returns bool - true if node list has been populated
    Q_INVOKABLE bool populated() const;
    // Populate list
    void populate();
    // Instantiate node from registry
    void instantiateNode(GraphModel *graphModel, QVariant type, QVariant name);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
