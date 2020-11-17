// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    // Defined integer parameters
    std::vector<std::shared_ptr<ExpressionVariable>> integerParameters_;
    // Defined double parameters
    std::vector<std::shared_ptr<ExpressionVariable>> doubleParameters_;

    public:
    // Add new integer parameter
    bool addParameter(std::string_view name, int initialValue = 0);
    // Add new double parameter
    bool addParameter(std::string_view name, double initialValue = 0.0);
    // Return whether this node has the named parameter specified
    std::shared_ptr<ExpressionVariable> hasParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter);
    // Return vector of all parameters for this node
    std::vector<std::shared_ptr<ExpressionVariable>> parameters() const;

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
