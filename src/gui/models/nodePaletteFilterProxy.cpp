// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/nodePaletteFilterProxy.h"
#include "procedure/nodes/registry.h"

// Set current context
void NodePaletteFilterProxy::setContext(ProcedureNode::NodeContext context)
{
    context_ = context;
    invalidateFilter();
}

/*
 * QSortFilterProxyModel overrides
 */

bool NodePaletteFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    auto category = std::next(ProcedureNodeRegistry::categoryMap().begin(), parent.isValid() ? parent.row() : row)->first;

    switch (context_)
    {
        case (ProcedureNode::NodeContext::AnyContext):
            return true;
        case (ProcedureNode::NodeContext::NoContext):
            return false;
        case (ProcedureNode::NodeContext::OperateContext):
            return category == "Operate";
        case (ProcedureNode::NodeContext::GenerationContext):
            return category != "Operate";
        case (ProcedureNode::NodeContext::AnalysisContext):
            return category != "Build" && category != "Potentials" && category != "Operate" && category != "Sites";
        default:
            throw(std::runtime_error(fmt::format("Context '{}' is not handled in the Node Palette.",
                                                 ProcedureNode::nodeContexts().keyword(context_))));
    }
}
