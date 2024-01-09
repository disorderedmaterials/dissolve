// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include <QSortFilterProxyModel>

class NodePaletteFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    private:
    // Current context
    ProcedureNode::NodeContext context_{ProcedureNode::NodeContext::NoContext};

    public:
    // Set current context
    void setContext(const ProcedureNode::NodeContext context);

    /*
     * QSortFilterProxyModel overrides
     */
    private:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;
};