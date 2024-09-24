// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractListModel>
#include <qtmetamacros.h>
#include <vector>
#include "graphNodeModel.h"
#include "graphEdgeModel.h"
#include "nodeWrapper.h"

class GraphModel : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(GraphNodeModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);

    public:
    GraphModel();
    std::vector<NodeWrapper> items;

    public:
    GraphEdgeModel *edges();
    GraphNodeModel *nodes();
    std::string typeName(nodeValue &);
    std::string typeIcon(nodeValue &);

    private:
    GraphNodeModel nodes_;
    GraphEdgeModel edges_;

    Q_SIGNALS:
    void graphChanged();
};
