// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/nodevector.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "procedure/nodes/select.h"

OperateSitePopulationNormaliseProcedureNode::OperateSitePopulationNormaliseProcedureNode(
    std::vector<std::shared_ptr<const SelectProcedureNode>> sites)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateSitePopulationNormalise), normalisationSites_(std::move(sites))
{
    // Create keywords - store the pointers to the superclasses for later use
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>("Control", "Site",
                                                          "Site(s) by which to normalise data based on their population",
                                                          normalisationSites_, this, ProcedureNode::NodeType::Select, false);
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateSitePopulationNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        (*targetData1D_) /= node->nAverageSites();

    return true;
}

// Operate on Data2D target
bool OperateSitePopulationNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        (*targetData2D_) /= node->nAverageSites();

    return true;
}

// Operate on Data3D target
bool OperateSitePopulationNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto node : normalisationSites_)
        (*targetData3D_) /= node->nAverageSites();

    return true;
}
