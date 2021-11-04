// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

OperateSitePopulationNormaliseProcedureNode::OperateSitePopulationNormaliseProcedureNode(
    std::vector<ConstNodeRef > nodes)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateSitePopulationNormalise)
{
    // Create keywords - store the pointers to the superclasses for later use
  keywords_.add("Control", new NodeVectorKeyword(shared_from_this(), ProcedureNode::NodeType::Select, false, nodes), "Site",
                  "Site(s) by which to normalise data based on their population");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateSitePopulationNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<ConstNodeRef >>("Site");
    for (const auto node : nodes)
    {
        auto selectNode = std::dynamic_pointer_cast<const SelectProcedureNode>(node);
        assert(selectNode);
        (*targetData1D_) /= selectNode->nAverageSites();
    }

    return true;
}

// Operate on Data2D target
bool OperateSitePopulationNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<ConstNodeRef >>("Site");
    for (const auto node : nodes)
    {
        auto selectNode = std::dynamic_pointer_cast<const SelectProcedureNode>(node);
        assert(selectNode);
        (*targetData2D_) /= selectNode->nAverageSites();
    }

    return true;
}

// Operate on Data3D target
bool OperateSitePopulationNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<ConstNodeRef >>("Site");
    for (const auto node : nodes)
    {
        auto selectNode = std::dynamic_pointer_cast<const SelectProcedureNode>(node);
        assert(selectNode);
        (*targetData3D_) /= selectNode->nAverageSites();
    }

    return true;
}
