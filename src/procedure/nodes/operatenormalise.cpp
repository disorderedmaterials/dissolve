// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/operatenormalise.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "math/integrator.h"

OperateNormaliseProcedureNode::OperateNormaliseProcedureNode(double value)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNormalise)
{
    keywords_.add("Control", new NodeValueKeyword(this, value), "Value", "Constant value to normalise to");
    keywords_.add("Control", new BoolKeyword(true), "Absolute", "Normalise absolute sum of values rather than direct sum");
}
OperateNormaliseProcedureNode::OperateNormaliseProcedureNode(int value)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNormalise)
{
    keywords_.add("Control", new NodeValueKeyword(this, value), "Value", "Constant value to normalise to");
    keywords_.add("Control", new BoolKeyword(true), "Absolute", "Normalise absolute sum of values rather than direct sum");
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNormaliseProcedureNode::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the value
    const auto value = keywords_.asDouble("Value");
    const bool absolute = keywords_.asBool("Absolute");

    // Get sum of absolute values
    double sum = absolute ? Integrator::absSum(*targetData1D_) : Integrator::sum(*targetData1D_);
    (*targetData1D_) /= sum;
    (*targetData1D_) *= value;

    return true;
}

// Operate on Data2D target
bool OperateNormaliseProcedureNode::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the value
    const auto value = keywords_.asDouble("Value");
    const bool absolute = keywords_.asBool("Absolute");

    // Get sum of absolute values
    double sum = absolute ? Integrator::absSum(*targetData2D_) : Integrator::sum(*targetData2D_);
    (*targetData2D_) /= sum;
    (*targetData2D_) *= value;

    return true;
}

// Operate on Data3D target
bool OperateNormaliseProcedureNode::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    // Evaluate the expression to get the value
    const auto value = keywords_.asDouble("Value");
    const bool absolute = keywords_.asBool("Absolute");

    // Get sum of absolute values
    double sum = absolute ? Integrator::absSum(*targetData3D_) : Integrator::sum(*targetData3D_);
    (*targetData3D_) /= sum;
    (*targetData3D_) *= value;

    return true;
}
