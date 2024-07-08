// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/parameters.h"
#include "expression/variable.h"
#include "keywords/expressionVariableVector.h"

ParametersProcedureNode::ParametersProcedureNode() : ProcedureNode(NodeType::Parameters)
{
    keywords_.setOrganisation("Options", "Data");
    keywords_.add<ExpressionVariableVectorKeyword>("Parameter", "Defined parameters", parameters_, this);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool ParametersProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool ParametersProcedureNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Execute node
bool ParametersProcedureNode::execute(const ProcedureContext &procedureContext) { return true; }

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue ParametersProcedureNode::serialise() const
{
    SerialisedValue result;
    for (auto &param : parameters_)
        result[std::string(param->baseName())] = param->value();
    return result;
}

// Read values from a serialisable value
void ParametersProcedureNode::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toMap(node,
          [this](const auto &key, const auto &value)
          {
              if (key != "type")
                  addParameter(key, toml::get<ExpressionValue>(value));
          });
}
