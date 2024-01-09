// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateNormalise.h"
#include "keywords/bool.h"
#include "keywords/nodeValue.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/integrator.h"

OperateNormaliseProcedureNode::OperateNormaliseProcedureNode(double value)
    : OperateProcedureNodeBase(ProcedureNode::NodeType::OperateNormalise), value_{value}
{
    keywords_.setOrganisation("Options", "Inputs");
    keywords_.add<NodeValueKeyword>("Value", "Constant value to normalise to", value_, this);
    keywords_.add<BoolKeyword>("Absolute", "Normalise absolute sum of values rather than direct sum", absolute_);
}

/*
 * Data Target (implements virtuals in OperateProcedureNodeBase)
 */

// Operate on Data1D target
bool OperateNormaliseProcedureNode::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    // Get sum of absolute values
    auto sum = absolute_ ? Integrator::absSum(*targetData1D_) : Integrator::sum(*targetData1D_);

    (*targetData1D_) /= sum;
    (*targetData1D_) *= value_.asDouble();

    return true;
}

// Operate on Data2D target
bool OperateNormaliseProcedureNode::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    // Get sum of absolute values
    auto sum = absolute_ ? Integrator::absSum(*targetData2D_) : Integrator::sum(*targetData2D_);
    (*targetData2D_) /= sum;
    (*targetData2D_) *= value_.asDouble();

    return true;
}

// Operate on Data3D target
bool OperateNormaliseProcedureNode::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    // Get sum of absolute values
    auto sum = absolute_ ? Integrator::absSum(*targetData3D_) : Integrator::sum(*targetData3D_);

    (*targetData3D_) /= sum;
    (*targetData3D_) *= value_.asDouble();

    return true;
}
