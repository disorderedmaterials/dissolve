// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <qabstractitemmodel.h>
#include <qvariant.h>

class GraphModel;

// GraphModel class)
class GraphNodeModel : QAbstractListModel
{
    friend GraphModel;

    public:
    GraphNodeModel(GraphModel *parent = nullptr) : parent_(parent) {}
    GraphNodeModel(const GraphNodeModel &other) : parent_(other.parent_) {}

    GraphNodeModel &operator=(const GraphNodeModel &other);

    bool operator!=(const GraphNodeModel &other);

    // This is the number of roles that GraphNodeModel will use.  This
    // ensures that we can add more roles (to ALL node types) in the
    // future without needing to have each class reassign its own roles.
    static const int ownedRoles = 5;

    // Number of nodes (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    // Labels for QML roles (required by QAbstractListModel)
    QHash<int, QByteArray> roleNames() const;

    // Data accessor (required by QAbstractListModel)
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // Data setter (required by QAbstractListModel)
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // Must call *before* inserting new elements.  The count is the number of elements that will be inserted
    void beginInsert(int count = 1);
    // Must call *after* inserting new elements
    void endInsert();
    // Alert that the parent graph has updated
    void updateGraph();

    private:
    // The GraphModel that this is part of (which will hold the actual vector of nodes
    GraphModel *parent_;
};
