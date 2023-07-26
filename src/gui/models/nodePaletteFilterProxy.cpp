// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/nodePaletteFilterProxy.h"
#include "procedure/nodes/registry.h"

// Set allowed categories
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
    if (context_ == ProcedureNode::NodeContext::AnyContext)
        return true;

    static std::map<ProcedureNode::NodeContext, std::vector<std::string>> contextCategories = {
        {ProcedureNode::NodeContext::NoContext, {}},
        {ProcedureNode::NodeContext::AnalysisContext, {"Data"}},
        {ProcedureNode::NodeContext::GenerationContext,
         {"Calculate", "Build", "Calculate", "Data", "General", "Operate", "Pick", "Potentials", "Sites"}},
        {ProcedureNode::NodeContext::OperateContext,
         {"Calculate", "Data", "General", "Operate", "Pick", "Potentials", "Sites"}},
    };

    const auto &contextCategoryList = contextCategories.at(context_);
    auto category = std::next(ProcedureNodeRegistry::categoryMap().begin(), parent.isValid() ? parent.row() : row)->first;
    return std::find(contextCategoryList.begin(), contextCategoryList.end(), category) != contextCategoryList.end();
}
