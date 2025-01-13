// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>
#include <qvariant.h>

template <Graphable T> class GraphModel;

// A base to add a static terms or properties to GraphNodeModel
class GraphNodeModelBase : public QAbstractListModel
{
    public:
    // This is the number of roles that GraphNodeModel will use.  This
    // ensures that we can add more roles (to ALL node types) in the
    // future without needing to have each class reassign its own roles.
    static const int ownedRoles = 5;
};

// The model for accessing the node data (which is *held* in the
// GraphModel class)
template <Graphable T> class GraphNodeModel : public GraphNodeModelBase
{
    friend GraphModel<T>;

    public:
    GraphNodeModel(GraphModel<T> *parent = nullptr) : parent_(parent) {}
    GraphNodeModel(const GraphNodeModel<T> &other) : parent_(other.parent_) {}

    GraphNodeModel<T> &operator=(const GraphNodeModel<T> &other)
    {
        parent_ = other.parent_;
        return *this;
    }
    bool operator!=(const GraphNodeModel<T> &other) { return &parent_ != &other.parent_; }

    // Number of nodes (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return parent_->items().size(); }

    // Labels for QML roles (required by QAbstractListModel)
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "name";
        roles[Qt::UserRole + 1] = "posX";
        roles[Qt::UserRole + 2] = "posY";
        roles[Qt::UserRole + 3] = "type";
        roles[Qt::UserRole + 4] = "icon";
        return nodeRoleNames<T>(roles);
    }

    // Data accessor (required by QAbstractListModel)
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {

        auto &item = parent_->items().at(index.row());
        switch (role - Qt::UserRole)
        {
            case 0:
                return nodeName(item.rawValue()).c_str();
            case 1:
                return item.posx;
            case 2:
                return item.posy;
            case 3:
                return nodeTypeName(item.rawValue()).c_str();
            case 4:
                return nodeTypeIcon(item.rawValue()).c_str();
            default:
                return nodeData(item.rawValue(), role - Qt::UserRole - ownedRoles);
        }
    }

    // Data setter (required by QAbstractListModel)
    bool setData(const QModelIndex &index, const QVariant &value, int role) override
    {
        auto &item = parent_->items().at(index.row());
        switch (role - Qt::UserRole)
        {
            case 0:
                setNodeName(item.rawValue(), value.toString().toStdString());
                return true;
            case 1:
                item.posx = value.toInt();
                return true;
            case 2:
                item.posy = value.toInt();
                return true;
            default:
                return nodeSetData(item.rawValue(), value, role - Qt::UserRole - ownedRoles);
        }
    }

    // Must call *before* inserting new elements.  The count is the number of elements that will be inserted
    void beginInsert(int count = 1) { beginInsertRows({}, parent_->items().size(), parent_->items().size() + count); }
    // Must call *after* inserting new elements
    void endInsert() { endInsertRows(); }

    private:
    // The GraphModel that this is part of (which will hold the actual vector of nodes
    GraphModel<T> *parent_;
};
