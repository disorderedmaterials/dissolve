// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "nodeWrapper.h"
#include <QAbstractListModel>
#include <qvariant.h>

template <typename T> class GraphModel;

/** A list model that provides the tick labels of the axis */
template <typename T> class GraphNodeModel : public QAbstractListModel
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
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return parent_->items.size(); }
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "name";
        roles[Qt::UserRole + 1] = "posX";
        roles[Qt::UserRole + 2] = "posY";
        roles[Qt::UserRole + 3] = "type";
        roles[Qt::UserRole + 4] = "icon";
        roles[Qt::UserRole + 5] = "value";
        return roles;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {

        auto &item = parent_->items.at(index.row());
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
            case 5:
                return item.value();
        }
        return {};
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role) override
    {
        auto &item = parent_->items.at(index.row());
        switch (role - Qt::UserRole)
        {
            case 1:
                item.posx = value.toInt();
                return true;
            case 2:
                item.posy = value.toInt();
                return true;
            default:
                return false;
        }
        return false;
    }
    /** Function to reset the model (and trigger redrawing all labels */

    private:
    GraphModel<T> *parent_;
};
