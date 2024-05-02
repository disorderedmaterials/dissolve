// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/operateBase.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

OperateProcedureNodeBase::OperateProcedureNodeBase(ProcedureNode::NodeType nodeType)
    : ProcedureNode(nodeType, {ProcedureNode::OperateContext}, ProcedureNode::NodeClass::Operate)
{
    targetData1D_ = nullptr;
    targetData2D_ = nullptr;
    targetData3D_ = nullptr;
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool OperateProcedureNodeBase::mustBeNamed() const { return false; }

/*
 * Data Target
 */

// Set target Data1D
void OperateProcedureNodeBase::setTarget(Data1D *target)
{
    targetData1D_ = target;
    targetData2D_ = nullptr;
    targetData3D_ = nullptr;
}

// Set target Data2D
void OperateProcedureNodeBase::setTarget(Data2D *target)
{
    targetData1D_ = nullptr;
    targetData2D_ = target;
    targetData3D_ = nullptr;
}

// Set target Data3D
void OperateProcedureNodeBase::setTarget(Data3D *target)
{
    targetData1D_ = nullptr;
    targetData2D_ = nullptr;
    targetData3D_ = target;
}

// Operate on Data1D target
bool OperateProcedureNodeBase::operateData1D(const ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 1-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

// Operate on Data2D target
bool OperateProcedureNodeBase::operateData2D(const ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 2-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

// Operate on Data3D target
bool OperateProcedureNodeBase::operateData3D(const ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 3-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool OperateProcedureNodeBase::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool OperateProcedureNodeBase::execute(const ProcedureContext &procedureContext)
{
    // Run the operation on any data target that exists
    if (targetData1D_ && (!operateData1D(procedureContext.processPool(), procedureContext.configuration())))
        return false;
    if (targetData2D_ && (!operateData2D(procedureContext.processPool(), procedureContext.configuration())))
        return false;
    if (targetData3D_ && (!operateData3D(procedureContext.processPool(), procedureContext.configuration())))
        return false;

    return true;
}
