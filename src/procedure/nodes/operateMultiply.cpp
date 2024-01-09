// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateMultiply.h"
#include "keywords/nodeValue.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"

OperateMultiplyProcedureNode::OperateMultiplyProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateMultiply)
{
    keywords_.setOrganisation("Options", "Inputs");
    keywords_.add<NodeValueKeyword>("Value", "Constant value to use as the multiplier", value_, this);
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateMultiplyProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    (*targetData1D_) *= value_.asDouble();

    return true;
}

// Operate on Data2D target
bool OperateMultiplyProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    (*targetData2D_) *= value_.asDouble();

    return true;
}

// Operate on Data3D target
bool OperateMultiplyProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    (*targetData3D_) *= value_.asDouble();

    return true;
}
