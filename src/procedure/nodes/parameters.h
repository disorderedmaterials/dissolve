// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/node.h"

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
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Parameters
     */
    public:
    // Add new parameter
    std::shared_ptr<ExpressionVariable> addParameter(std::string_view name, ExpressionValue initialValue);
    // Return the named parameter (if it exists)
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter) override;
    // Return vector of all parameters for this node
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> parameters() const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
