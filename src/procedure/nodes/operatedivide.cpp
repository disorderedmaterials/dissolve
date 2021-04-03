// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatedivide.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"
#include "math/data1d.h"

OperateDivideProcedureNode::OperateDivideProcedureNode() : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateDivide)
{
    keywords_.add("Control", new NodeValueKeyword(this, 1.0), "Value", "Constant value to use as the divisor");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateDivideProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the divisor
    const auto divisor = keywords_.asDouble("Value");
    (*targetData1D_) /= divisor;

    return true;
}

// Operate on Data2D target
bool OperateDivideProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the divisor
    const auto divisor = keywords_.asDouble("Value");
    (*targetData2D_) /= divisor;

    return true;
}

// Operate on Data3D target
bool OperateDivideProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the divisor
    const auto divisor = keywords_.asDouble("Value");
    (*targetData3D_) /= divisor;

    return true;
}
