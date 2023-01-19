// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/parameters.h"
#include "expression/variable.h"
#include "keywords/expressionvariablevector.h"

ParametersProcedureNode::ParametersProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Parameters)
{
    keywords_.add<ExpressionVariableVectorKeyword>("Control", "Parameter", "Defined parameters", parameters_, this);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool ParametersProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context != ProcedureNode::NoContext);
}

// Return whether a name for the node must be provided
bool ParametersProcedureNode::mustBeNamed() const { return false; }

/*
 * Parameters
 */

// Add new parameter
std::shared_ptr<ExpressionVariable> ParametersProcedureNode::addParameter(std::string_view name, ExpressionValue initialValue)
{
    return parameters_.emplace_back(std::make_shared<ExpressionVariable>(name, initialValue));
}

// Return the named parameter (if it exists)
std::shared_ptr<ExpressionVariable> ParametersProcedureNode::getParameter(std::string_view name,
                                                                          std::shared_ptr<ExpressionVariable> excludeParameter)
{
    for (auto var : parameters_)
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;

    return nullptr;
}

// Return vector of all parameters for this node
OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> ParametersProcedureNode::parameters() const
{
    return parameters_;
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool ParametersProcedureNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool ParametersProcedureNode::execute(const ProcedureContext &procedureContext) { return true; }

// Express as a serialisable value
SerialisedValue ParametersProcedureNode::serialise() const
{
    SerialisedValue result;
    for (auto &param : parameters_)
        result[std::string(param->name())] = param->value();
    return result;
}

// Read values from a serialisable value
void ParametersProcedureNode::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto &[k, v] : node.as_table())
        if (k != "type")
            parameters_.push_back(std::make_shared<ExpressionVariable>(k, toml::get<ExpressionValue>(v)));
}
