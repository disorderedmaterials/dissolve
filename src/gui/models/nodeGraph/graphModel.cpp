// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "graphModel.h"
#include <qnamespace.h>
#include <qvariant.h>

GraphEdgeModel *GraphModel::edges() { return &edges_; }
GraphNodeModel *GraphModel::nodes() { return &nodes_; }

std::string GraphModel::typeName(nodeValue &value)
{
    switch (value.index())
    {
        case 0:
        case 1:
            return "number";
        default:
            return "";
    }
}

std::string GraphModel::typeIcon(nodeValue &value)
{
    switch (value.index())
    {
        case 0:
        case 1:
            return "file:/home/adam/Code/dissolve/src/gui/icons/open.svg";
        default:
            return "";
    }
}
