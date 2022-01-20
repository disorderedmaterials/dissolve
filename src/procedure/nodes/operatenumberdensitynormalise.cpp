// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/operatenumberdensitynormalise.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(std::vector<const ProcedureNode *> nodes)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise)
{
    // Create keywords
    keywords_.add("Control", new NodeVectorKeyword(this, ProcedureNode::NodeType::Select, false, nodes), "Site",
                  "Site(s) by which to normalise data based on their population");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<const ProcedureNode *>>("Site");
    for (const auto *node : nodes)
    {
        auto *selectNode = dynamic_cast<const SelectProcedureNode *>(node);
        assert(selectNode);
        (*targetData1D_) /= (selectNode->nAverageSites() / cfg->box()->volume());
    }

    return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<const ProcedureNode *>>("Site");
    for (const auto *node : nodes)
    {
        auto *selectNode = dynamic_cast<const SelectProcedureNode *>(node);
        assert(selectNode);
        (*targetData2D_) /= (selectNode->nAverageSites() / cfg->box()->volume());
    }

    return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    auto &nodes = keywords_.retrieve<std::vector<const ProcedureNode *>>("Site");
    for (const auto *node : nodes)
    {
        auto *selectNode = dynamic_cast<const SelectProcedureNode *>(node);
        assert(selectNode);
        (*targetData3D_) /= (selectNode->nAverageSites() / cfg->box()->volume());
    }

    return true;
}
