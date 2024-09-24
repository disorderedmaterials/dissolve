// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>

class GraphModel;

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
