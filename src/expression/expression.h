/*
	*** Mathematical Expression
	*** src/expression/expression.h
	Copyright T. Youngs 2015-2019

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

#ifndef DISSOLVE_EXPRESSION_H
#define DISSOLVE_EXPRESSION_H

#include "expression/node.h"
#include "expression/functions.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward declarations
class ExpressionVariable;
class Node;

// Mathematical Expression
class Expression
{
	public:
	// Constructor
	Expression(const char* expressionText = NULL);
	// Destructor
	~Expression();
	// Copy constructor
	Expression(const Expression& source);
	// Assignment operator
	void operator=(const Expression& source);


	/*
	 * Data
	 */
	private:
	// Original generating string
	CharString expressionString_;

	public:
	// Clear all expression data
	void clear();
	// Return whether current expression is valid (contains at least one node)
	bool isValid();
	// Set Expression from supplied string
	bool set(const char* expressionString);
	// Set Expression from supplied string and external variables
	bool set(const char* expressionString, RefList<ExpressionVariable> externalVariables);
	// Return original generating string`
	const char* expressionString() const;


	/*
	 * Nodes
	 */
	private:
	// Node list - a disordered list of all nodes (except persistent ones) owned by the Expression
	List<ExpressionNode> nodes_;
	// Persistent node list, not removed by normal clear() function
	List<ExpressionNode> persistentNodes_;
	// Reflist of all statements in the Expression, to be executed sequentially
	RefList<ExpressionNode> statements_;

	public:
	// Add a node representing a whole statement to the execution list
	bool addStatement(ExpressionNode* node);
	// Add an operator to the Expression
	ExpressionNode* addOperator(ExpressionFunctions::Function func, ExpressionNode* arg1, ExpressionNode* arg2 = NULL);
	// Associate a command-based node to the Expression
	ExpressionNode* addFunctionNodeWithArglist(ExpressionFunctions::Function func, ExpressionNode* arglist);
	// Add a function node to the list (overloaded to accept simple arguments instead of a list)
	ExpressionNode* addFunctionNode(ExpressionFunctions::Function func, ExpressionNode* arg1 = NULL, ExpressionNode* arg2 = NULL, ExpressionNode* arg3 = NULL, ExpressionNode* arg4 = NULL);
	// Add a value node, targetting the supplied variable
	ExpressionNode* addValueNode(ExpressionVariable* var);
	// Join two nodes together
	static ExpressionNode* joinArguments(ExpressionNode* arg1, ExpressionNode* arg2);
	// Join two commands together
	ExpressionNode* joinCommands(ExpressionNode* node1, ExpressionNode* node2);
	// Print statement info
	void print();


	/*
	 * Variables / Constants
	 */
	private:
	// Reference list of variables
	RefList<ExpressionVariable> variables_;
	// Reference list of constants
	RefList<ExpressionVariable> constants_;
	// Reference list of external variables
	RefList<ExpressionVariable> externalVariables_;

	public:
	// Create numeric constant
	ExpressionVariable* createConstant(ExpressionValue value, bool persistent = false);
	// Create integer variable, with optional ExpressionNode as initial value source
	ExpressionVariable* createIntegerVariable(const char* name, bool persistent = false, ExpressionNode* initialValue = NULL);
	// Create double variable, with optional ExpressionNode as initial value source
	ExpressionVariable* createDoubleVariable(const char* name, bool persistent = false, ExpressionNode* initialValue = NULL);
	// Create variable with supplied initial value
	ExpressionVariable* createVariableWithValue(const char* name, ExpressionValue initialValue, bool persistent = false);
	// Set list of external variables
	void setExternalVariables(RefList<ExpressionVariable> externalVariables);
	// Search for variable
	ExpressionVariable* variable(const char* name);
	// Return list of variables
	RefList<ExpressionVariable>& variables();
	// Return list of constants
	RefList<ExpressionVariable>& constants();


	/*
	 * Execution
	 */
	public:
	// Execute, returning whether successful, and setting result value of some type
	bool execute(ExpressionValue& result);
	// Execute and return as integer
	int asInteger();
	// Execute and return as double
	double asDouble();
};

#endif
