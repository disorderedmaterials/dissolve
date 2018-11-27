/*
	*** Mathematical Expression
	*** src/expression/expression.h
	Copyright T. Youngs 2015-2018

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

// External declarations
extern int ExpressionParser_parse();

// Forward declarations
class Node;
class ExpressionVariable;

// Mathematical Expression
class Expression
{
	public:
	// Constructor / Destructor
	Expression();
	Expression(const char* expression);
	~Expression();


	/*
	 * Creation
	 */
	public:
	// Multi-Character Symbols
	enum SymbolToken { AssignSymbol, GEQSymbol, LEQSymbol, CNEQSymbol, FNEQSymbol, AndSymbol, OrSymbol, nSymbolTokens };

	private:
	// Character string source
	CharString stringSource_;
	// Integer position in stringSource, total length of string, and starting position of current token/function
	int stringPos_, stringLength_, tokenStart_, functionStart_;
	// Whether to use additional pre-defined constants
	bool useAdditionalConstants_;
	// Flag to specify that missing variables should be generated
	bool generateMissingVariables_;
	// Whether current expression is valid
	bool isValid_;
	// Target for expression generation
	static Expression* target_;

	public:
	// Reset structure ready for next source
	void resetParser();
	// Clear all expression data
	void clear();
	// Set whether missing variables should be generated
	void setGenerateMissingVariables(bool generate);
	// Return whether missing variables will be generated
	bool generateMissingVariables();
	// Parser lexer, called by yylex()
	int lex();
	// Get next character from current input stream
	char getChar();
	// Peek next character from current input stream
	char peekChar();
	// 'Replace' last character read from current input stream
	void unGetChar();
	// Perform expression generation for this expression
	bool generate(const char* expressionText);
	// Return whether current expression is valid
	bool isValid();
	// Return current expression target
	static Expression* target();


	/*
	 * Node Control
	 */
	private:
	// Node list - a disordered list of all nodes (except persistent ones) owned by the Expression
	List<ExpressionNode> nodes_;
	// Persistent node list, not removed by normal clear() function
	List<ExpressionNode> persistentNodes_;
	// Reflist of all statements in the Expression, to be executed sequentially
	RefList<ExpressionNode,int> statements_;
	// Number of syntactic errors encountered
	int nErrors_;

	public:
	// Add a node representing a whole statement to the execution list
	bool addStatement(ExpressionNode* leaf);
	// Add an operator to the Expression
	ExpressionNode* addOperator(ExpressionFunctions::Function func, ExpressionNode* arg1, ExpressionNode* arg2 = NULL);
	// Associate a command-based leaf node to the Expression
	ExpressionNode* addFunctionNodeWithArglist(ExpressionFunctions::Function func, ExpressionNode* arglist);
	// Add a function node to the list (overloaded to accept simple arguments instead of a list)
	ExpressionNode* addFunctionNode(ExpressionFunctions::Function func, ExpressionNode* a1 = NULL, ExpressionNode* a2 = NULL, ExpressionNode* a3 = NULL, ExpressionNode* a4 = NULL);
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
	// List of variables
	RefList<ExpressionVariable,bool> variables_;
	// List of constants
	RefList<ExpressionVariable,bool> constants_;

	public:
	// Add double constant
	ExpressionVariable* createConstant(double d, bool persistent = false);
	// Add variable, with ExpressionNode as initial value source
	ExpressionVariable* createVariable(const char* name, bool persistent = false, ExpressionNode* initialValue = NULL);
	// Add variable, with double as initial value source
	ExpressionVariable* createVariableWithValue(const char* name, double initialValue, bool persistent = false);
	// Search for variable
	ExpressionVariable* variable(const char* name);
	// Return variables
	RefListItem<ExpressionVariable,bool>* variables();
	// Return constants
	RefListItem<ExpressionVariable,bool>* constants();


	/*
	 * Execution
	 */
	public:
	// Execute
	double execute(bool& success);
};

#endif
