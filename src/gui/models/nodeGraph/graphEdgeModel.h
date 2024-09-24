// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>

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

    private:
    GraphModel *parent_;
};
