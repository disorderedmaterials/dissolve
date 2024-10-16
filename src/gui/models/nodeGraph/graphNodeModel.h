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
      return nodeRoleNames<T>();
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {

        auto &item = parent_->items.at(index.row());
        return nodeData(item, role);
    }
    bool setData(const QModelIndex &index, const QVariant &value, int role) override
    {
        auto &item = parent_->items.at(index.row());
        return nodeSetData(item, value, role);
    }
    /** Function to reset the model (and trigger redrawing all labels */

    private:
    GraphModel<T> *parent_;
};
