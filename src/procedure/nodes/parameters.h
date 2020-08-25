/*
    *** Procedure Node - Parameters
    *** src/procedure/nodes/parameters.h
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

#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/array.h"

// Forward Declarations
class ExpressionVariable;

// Parameters Node
class ParametersProcedureNode : public ProcedureNode
{
    public:
    ParametersProcedureNode();
    ~ParametersProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Parameters
     */
    private:
    // List of defined integer parameters (as base ExpressionNode type)
    List<ExpressionNode> integerParameters_;
    // List of defined double parameters (as base ExpressionNode type)
    List<ExpressionNode> doubleParameters_;

    public:
    // Add new integer parameter
    bool addParameter(std::string_view name, int initialValue = 0);
    // Add new double parameter
    bool addParameter(std::string_view name, double initialValue = 0.0);
    // Return whether this node has the named parameter specified
    ExpressionVariable *hasParameter(std::string_view name, ExpressionVariable *excludeParameter);
    // Return list of all parameters for this node
    RefList<ExpressionVariable> parameterReferences() const;

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
