/*
	*** Keyword - Species Reference List
	*** src/keywords/speciesreferencelist.cpp
	Copyright T. Youngs 2012-2019

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

#include "keywords/expressionvariablelist.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/node.h"
#include "base/lineparser.h"

// Constructor
ExpressionVariableListKeyword::ExpressionVariableListKeyword(ProcedureNode* parentNode, List<ExpressionNode>& variables, ExpressionValue::ValueType variableType) : KeywordData< List<ExpressionNode>& >(KeywordBase::ExpressionVariableListData, variables)
{
	parentNode_ = parentNode;
	variableType_ = variableType;
}

// Destructor
ExpressionVariableListKeyword::~ExpressionVariableListKeyword()
{
}

/*
 * Parent Node
 */

// Return parent ProcedureNode
const ProcedureNode* ExpressionVariableListKeyword::parentNode() const
{
	return parentNode_;
}

/*
 * Variable Type
 */

// Return assumed type for variables in the list
ExpressionValue::ValueType ExpressionVariableListKeyword::variableType() const
{
	return variableType_;
}

/*
 * Data
 */

// Determine whether current data is actually 'set'
bool ExpressionVariableListKeyword::currentDataIsSet() const
{
	return data_.nItems() > 0;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ExpressionVariableListKeyword::minArguments() const
{
	return 2;
}

// Return maximum number of arguments accepted
int ExpressionVariableListKeyword::maxArguments() const
{
	return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool ExpressionVariableListKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	if (!parentNode_) return Messenger::error("Parent ProcedureNode not set, so can't read ExpressionVariableList data.\n");

	// First argument is the name of the parameter to create - does it already exist in the node's current scope?
	ExpressionVariable* parameter = parentNode_->parameterInScope(parser.argc(startArg));
	if (parameter) return Messenger::error("A parameter with the name '%s' is already in scope, and cannot be redefined.\n", parser.argc(startArg));

	// Create a new one
	parameter = new ExpressionVariable;
	data_.own(parameter);
	parameter->setName(parser.argc(startArg));

	// Set the initial value
	if (variableType_ == ExpressionValue::IntegerType)
	{
		if (!parameter->set(parser.argi(startArg+1))) return Messenger::error("Failed to set initial value for parameter '%s'.\n", parser.argc(startArg));
	}
	else if (variableType_ == ExpressionValue::DoubleType)
	{
		if (!parameter->set(parser.argd(startArg+1))) return Messenger::error("Failed to set initial value for parameter '%s'.\n", parser.argc(startArg));
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool ExpressionVariableListKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of defined ExpressionNode's (ExpressionVariables)
	ListIterator<ExpressionNode> nodeIterator(data_);
	while (ExpressionNode* node = nodeIterator.iterate())
	{
		// Cast up to ExpressionVariable
		ExpressionVariable* var = dynamic_cast<ExpressionVariable*>(node);
		if (!var) Messenger::error("Failed to cast ExpressionNode to ExpressionVariable when writing ExpressionVariableList data.\n");
		else if (variableType_ == ExpressionValue::IntegerType)
		{
			if (!parser.writeLineF("%s%s  %s  %i\n", prefix, name(), var->name(), var->value().asInteger())) return false;
		}
		else if (variableType_ == ExpressionValue::DoubleType)
		{
			if (!parser.writeLineF("%s%s  %s  %12.6e\n", prefix, name(), var->name(), var->value().asDouble())) return false;
		}
	}

	return true;
}

/*
 * Object Management
 */
