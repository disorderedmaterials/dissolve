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

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise)
{
    // Create keywords - store the pointers to the superclasses for later use
    keywords_.add("Control",
                  new NodeRefListKeyword<const SelectProcedureNode>(this, ProcedureNode::NodeType::Select, false, selectNodes_),
                  "Site", "Site(s) by which to normalise data based on their population");
}

OperateNumberDensityNormaliseProcedureNode::OperateNumberDensityNormaliseProcedureNode(
    RefList<const SelectProcedureNode> selectNodes)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNumberDensityNormalise)
{
    // Create keywords - store the pointers to the superclasses for later use
    keywords_.add("Control",
                  new NodeRefListKeyword<const SelectProcedureNode>(this, ProcedureNode::NodeType::Select, false, selectNodes_),
                  "Site", "Site(s) by which to normalise data based on their population");

    selectNodes_ = selectNodes;
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNumberDensityNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    for (const SelectProcedureNode *selectNode : selectNodes_)
        (*targetData1D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

    return true;
}

// Operate on Data2D target
bool OperateNumberDensityNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    for (const SelectProcedureNode *selectNode : selectNodes_)
        (*targetData2D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

    return true;
}

// Operate on Data3D target
bool OperateNumberDensityNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    for (const SelectProcedureNode *selectNode : selectNodes_)
        (*targetData3D_) /= (selectNode->nAverageSites() / cfg->box()->volume());

    return true;
}
