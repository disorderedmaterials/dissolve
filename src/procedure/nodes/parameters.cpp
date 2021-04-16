// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/parameters.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "expression/variable.h"
#include "keywords/types.h"

ParametersProcedureNode::ParametersProcedureNode() : ProcedureNode(ProcedureNode::NodeType::Parameters)
{
    keywords_.add("Control", new ExpressionVariableVectorKeyword(this, parameters_), "Parameter", "Defined parameters");
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
void ParametersProcedureNode::addParameter(std::string_view name, ExpressionValue initialValue)
{
    parameters_.push_back(std::make_shared<ExpressionVariable>(name, initialValue));
}

// Return whether this node has the named parameter specified
std::shared_ptr<ExpressionVariable> ParametersProcedureNode::hasParameter(std::string_view name,
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
bool ParametersProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
bool ParametersProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    return true;
}
