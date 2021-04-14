// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ~ParametersProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const;

    /*
     * Parameters
     */
    private:
    // Defined parameters
    std::vector<std::shared_ptr<ExpressionVariable>> parameters_;

    public:
    // Add new parameter
    void addParameter(std::string_view name, ExpressionValue initialValue);
    // Return whether this node has the named parameter specified
    std::shared_ptr<ExpressionVariable> hasParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter);
    // Return vector of all parameters for this node
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> parameters() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
