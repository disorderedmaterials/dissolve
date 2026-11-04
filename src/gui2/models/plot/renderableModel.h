// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/plot/plot1DModel.h"
#include "math/data1D.h"
#include "nodes/node.h"
#include <QAbstractListModel>

class RenderableModel : public QAbstractListModel
{
    Q_OBJECT;

    public:
    RenderableModel(const Node::NodeParameterMap &outputs);
    ~RenderableModel() = default;

    struct RenderableData
    {
        std::string name;
        std::function<const Data1D *()> lambda;
    };

    enum Roles
    {
        KEY,
        DATA,
        TYPE
    };

    public:
    // Set source 1D data
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    private:
    // Renderable data
    std::vector<std::pair<std::string, std::unique_ptr<Plot1DModel>>> dataSets_;

    public Q_SLOTS:
    // Update all renderable data
    void update();
    // Returns bool - true if there are no renderables associated with this node
    bool empty();

    /*
     * QAbstractListModel overrides
     */

    public:
    // Return number of edges (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;
};