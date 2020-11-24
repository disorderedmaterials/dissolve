// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "procedure/nodes/parameters.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "expression/variable.h"
#include "keywords/types.h"

ParametersProcedureNode::ParametersProcedureNode() : ProcedureNode(ProcedureNode::ParametersNode)
{
    keywords_.add("Defined Parameters", new ExpressionVariableVectorKeyword(this, parameters_, ExpressionValue::IntegerType),
                  "Integer", "Available integer parameters");
    keywords_.add("Defined Parameters", new ExpressionVariableVectorKeyword(this, parameters_, ExpressionValue::DoubleType),
                  "Real", "Available real (floating point) parameters");
}

ParametersProcedureNode::~ParametersProcedureNode() {}

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

// Add new integer parameter
bool ParametersProcedureNode::addParameter(std::string_view name, int initialValue)
{
    parameters_.push_back(std::make_shared<ExpressionVariable>(name, initialValue));

    return true;
}

// Add new double parameter
bool ParametersProcedureNode::addParameter(std::string_view name, double initialValue)
{
    parameters_.push_back(std::make_shared<ExpressionVariable>(name, initialValue));

    return true;
}

// Return whether this node has the named parameter specified
std::shared_ptr<ExpressionVariable> ParametersProcedureNode::hasParameter(std::string_view name,
                                                                          std::shared_ptr<ExpressionVariable> excludeParameter)
{
    // Search integer parameters
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
bool ParametersProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult ParametersProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                    std::string_view prefix, GenericList &targetList)
{
    return ProcedureNode::Success;
}
