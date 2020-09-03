/*
    *** Procedure Node - Operate Node Base
    *** src/procedure/nodes/operatenode.h
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

#pragma once

#include "keywords/node.h"
#include "procedure/nodes/node.h"
#include "templates/array.h"

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;

// Operate Node Base
class OperateProcedureNodeBase : public ProcedureNode
{
    public:
    OperateProcedureNodeBase(ProcedureNode::NodeType nodeType);
    virtual ~OperateProcedureNodeBase();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Data Target
     */
    protected:
    // Target Data1D (if relevant)
    Data1D *targetData1D_;
    // Target Data2D (if relevant)
    Data2D *targetData2D_;
    // Target Data3D (if relevant)
    Data3D *targetData3D_;

    public:
    // Set target Data1D
    void setTarget(Data1D *target);
    // Set target Data2D
    void setTarget(Data2D *target);
    // Set target Data3D
    void setTarget(Data3D *target);
    // Operate on Data1D target
    virtual bool operateData1D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data2D target
    virtual bool operateData2D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data3D target
    virtual bool operateData3D(ProcessPool &procPool, Configuration *cfg);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
