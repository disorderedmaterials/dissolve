/*
    *** Procedure Node - Parameters
    *** src/procedure/nodes/parameters.cpp
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

#include "procedure/nodes/parameters.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "expression/variable.h"
#include "keywords/types.h"

ParametersProcedureNode::ParametersProcedureNode() : ProcedureNode(ProcedureNode::ParametersNode)
{
    keywords_.add("Defined Parameters",
                  new ExpressionVariableListKeyword(this, integerParameters_, ExpressionValue::IntegerType), "Integer",
                  "Available integer parameters");
    keywords_.add("Defined Parameters", new ExpressionVariableListKeyword(this, doubleParameters_, ExpressionValue::DoubleType),
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
    // Create a new one
    ExpressionVariable *parameter = new ExpressionVariable;
    integerParameters_.own(parameter);
    parameter->setName(name);

    // Set the initial value
    if (!parameter->set(initialValue))
        return Messenger::error("Failed to set initial value for parameter '{}'.\n", name);

    return true;
}

// Add new double parameter
bool ParametersProcedureNode::addParameter(std::string_view name, double initialValue)
{
    // Create a new one
    ExpressionVariable *parameter = new ExpressionVariable;
    doubleParameters_.own(parameter);
    parameter->setName(name);

    // Set the initial value
    if (!parameter->set(initialValue))
        return Messenger::error("Failed to set initial value for parameter '{}'.\n", name);

    return true;
}

// Return whether this node has the named parameter specified
ExpressionVariable *ParametersProcedureNode::hasParameter(std::string_view name, ExpressionVariable *excludeParameter)
{
    // Search integer parameters
    ListIterator<ExpressionNode> integerIterator(integerParameters_);
    while (ExpressionNode *node = integerIterator.iterate())
    {
        // Cast up the node into an ExpressionVariable
        auto *var = dynamic_cast<ExpressionVariable *>(node);
        if (!var)
            continue;
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;
    }

    // Search double parameters
    ListIterator<ExpressionNode> doubleIterator(doubleParameters_);
    while (ExpressionNode *node = doubleIterator.iterate())
    {
        // Cast up the node into an ExpressionVariable
        auto *var = dynamic_cast<ExpressionVariable *>(node);
        if (!var)
            continue;
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;
    }

    return nullptr;
}

// Return list of all parameters for this node
RefList<ExpressionVariable> ParametersProcedureNode::parameterReferences() const
{
    RefList<ExpressionVariable> params;

    // Add integer parameters
    ListIterator<ExpressionNode> integerIterator(integerParameters_);
    while (ExpressionNode *node = integerIterator.iterate())
    {
        // Cast up the node into an ExpressionVariable
        auto *var = dynamic_cast<ExpressionVariable *>(node);
        if (var)
            params.append(var);
    }

    // Add double parameters
    ListIterator<ExpressionNode> doubleIterator(doubleParameters_);
    while (ExpressionNode *node = doubleIterator.iterate())
    {
        // Cast up the node into an ExpressionVariable
        auto *var = dynamic_cast<ExpressionVariable *>(node);
        if (var)
            params.append(var);
    }

    return params;
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
