// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphEdgeModel.h"
#include "graphNodeModel.h"
#include <QAbstractListModel>
#include <qtmetamacros.h>

class GraphModelBase : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(QAbstractListModel *nodes READ nodes NOTIFY graphChanged);
    Q_PROPERTY(GraphEdgeModel *edges READ edges NOTIFY graphChanged);
    Q_PROPERTY(int nodeCount READ count NOTIFY graphChanged);
    Q_PROPERTY(int edgeCount READ nEdges NOTIFY graphChanged);

    public:
    GraphModelBase();
    GraphEdgeModel *edges();
    virtual QAbstractListModel *nodes();
    virtual int count();
    int nEdges();

    protected:
    GraphEdgeModel edges_;

    private:
    virtual bool isValidEdgeSource_(int source, int sourceIndex, int destination, int destinationIndex);
    virtual bool connect_(int source, int sourceIndex, int destination, int destinationIndex);
    virtual bool disconnect_(int source, int sourceIndex, int destination, int destinationIndex);

    Q_SIGNALS:
    void graphChanged();

    public Q_SLOTS:
    virtual void emplace_back(int x, int y, QVariant value) {}
    virtual void deleteNode(int index) {}
    bool connect(int source, int sourceIndex, int destination, int destinationIndex);
    bool disconnect(int source, int sourceIndex, int destination, int destinationIndex);
};
