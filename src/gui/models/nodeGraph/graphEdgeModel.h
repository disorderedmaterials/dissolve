// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>

template <typename T> class GraphModel;

/** A list model that provides the tick labels of the axis */
template <typename T> class GraphEdgeModel : public QAbstractListModel
{
    friend GraphModel<T>;

    public:
    GraphEdgeModel(GraphModel<T> *parent = nullptr) : parent_(parent) {}
    GraphEdgeModel(const GraphEdgeModel<T> &other) : parent_(other.parent_) {}

    GraphEdgeModel<T> &operator=(const GraphEdgeModel<T> &other)
    {
        parent_ = other.parent_;
        return *this;
    }
    bool operator!=(const GraphEdgeModel<T> &other) { return parent_ != other.parent_; }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return parent_->items.size() / 2; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        auto row = index.row();

        switch (role - Qt::UserRole)
        {
            case 0:
                return row * 2;
            case 1:
                return 0;
            case 2:
                return row * 2 + 1;
            case 3:
                return 0;
            default:
                return {};
        }
    }
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::UserRole] = "source";
        roles[Qt::UserRole + 1] = "sourceIndex";
        roles[Qt::UserRole + 2] = "destination";
        roles[Qt::UserRole + 3] = "destIndex";
        return roles;
    }

    private:
    GraphModel<T> *parent_;
};
