// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(
    std::vector<const SelectProcedureNode *> nodes)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise), normalisationSites_(std::move(nodes))
{
    // Create keywords
    keywords_.add<NodeVectorKeyword<SelectProcedureNode>>("Control", "Site",
                                                          "Site(s) by which to normalise data based on their population",
                                                          normalisationSites_, this, ProcedureNode::NodeType::Select, false);
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto *node : normalisationSites_)
        (*targetData1D_) /= (node->nAverageSites() / cfg->box()->volume());

    return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto *node : normalisationSites_)
        (*targetData2D_) /= (node->nAverageSites() / cfg->box()->volume());

    return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    for (const auto *node : normalisationSites_)
        (*targetData3D_) /= (node->nAverageSites() / cfg->box()->volume());

    return true;
}
