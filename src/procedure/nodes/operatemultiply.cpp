// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatemultiply.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"
#include "math/data1d.h"

OperateMultiplyProcedureNode::OperateMultiplyProcedureNode()
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateMultiply)
{
    keywords_.add("Control", new NodeValueKeyword(this, 1.0), "Value", "Constant value to use as the multiplier");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateMultiplyProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the multiplier
    const auto multiplier = keywords_.asDouble("Value");
    (*targetData1D_) *= multiplier;

    return true;
}

// Operate on Data2D target
bool OperateMultiplyProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the multiplier
    const auto multiplier = keywords_.asDouble("Value");
    (*targetData2D_) *= multiplier;

    return true;
}

// Operate on Data3D target
bool OperateMultiplyProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the multiplier
    const auto multiplier = keywords_.asDouble("Value");
    (*targetData3D_) *= multiplier;

    return true;
}
