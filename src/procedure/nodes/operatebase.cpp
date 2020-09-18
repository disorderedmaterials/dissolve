/*
    *** Procedure Node - Operate Base Node
    *** src/procedure/nodes/operatebase.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/operatebase.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

OperateProcedureNodeBase::OperateProcedureNodeBase(ProcedureNode::NodeType nodeType) : ProcedureNode(nodeType)
{
    targetData1D_ = nullptr;
    targetData2D_ = nullptr;
    targetData3D_ = nullptr;
}

OperateProcedureNodeBase::~OperateProcedureNodeBase() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool OperateProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::OperateContext);
}

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
bool OperateProcedureNodeBase::operateData1D(ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 1-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

// Operate on Data2D target
bool OperateProcedureNodeBase::operateData2D(ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 2-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

// Operate on Data3D target
bool OperateProcedureNodeBase::operateData3D(ProcessPool &procPool, Configuration *cfg)
{
    return Messenger::error("The {} node can't operate on 3-dimensional data.\n", ProcedureNode::nodeTypes().keyword(type_));
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool OperateProcedureNodeBase::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult OperateProcedureNodeBase::execute(ProcessPool &procPool, Configuration *cfg,
                                                                     std::string_view prefix, GenericList &targetList)
{
    // Run the operation on any data target that exists
    if (targetData1D_ && (!operateData1D(procPool, cfg)))
        return ProcedureNode::Failure;
    if (targetData2D_ && (!operateData2D(procPool, cfg)))
        return ProcedureNode::Failure;
    if (targetData3D_ && (!operateData3D(procPool, cfg)))
        return ProcedureNode::Failure;

    return ProcedureNode::Success;
}
