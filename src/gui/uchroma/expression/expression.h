/*
	*** Expression
	*** src/gui/uchroma/expression/expression.h
	Copyright T. Youngs 2015

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_UCHROMAEXPRESSION_H
#define DISSOLVE_UCHROMAEXPRESSION_H

#include "gui/uchroma/expression/node.h"
#include "gui/uchroma/expression/functions.h"
#include "base/charstring.h"
#include "templates/list.h"

// External declarations
extern int ExpressionParser_parse();

// Forward declarations
class Node;
class Variable;

// Expression
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
	// Symbolic tokens - array of corresponding values refers to Bison's tokens
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
	// Node list - a disordered list of all nodes owned by the Expression
	List<Node> nodes_;
	// Permanent node list, not removed by normal clear() function
	List<Node> permanentNodes_;
	// Reflist of all statements in the Expression, to be executed sequentially
	RefList<Node,int> statements_;
	// Number of syntactic errors encountered
	int nErrors_;

	public:
	// Add a node representing a whole statement to the execution list
	bool addStatement(Node* leaf);
	// Add an operator to the Expression
	Node* addOperator(Functions::Function func, Node* arg1, Node* arg2 = NULL);
	// Associate a command-based leaf node to the Expression
	Node* addFunctionNodeWithArglist(Functions::Function func, Node* arglist);
	// Add a function node to the list (overloaded to accept simple arguments instead of a list)
	Node* addFunctionNode(Functions::Function func, Node* a1 = NULL, Node* a2 = NULL, Node* a3 = NULL, Node* a4 = NULL);
	// Add a variable node, targetting the supplied variable
	Node* addVariableNode(Variable* var);
	// Join two nodes together
	static Node* joinArguments(Node* arg1, Node* arg2);
	// Join two commands together
	Node* joinCommands(Node* node1, Node* node2);
	// Print statement info
	void print();


	/*
	 * Variables / Constants
	 */
	private:
	// List of variables
	RefList<Variable,bool> variables_;
	// List of constants
	RefList<Variable,bool> constants_;

	public:
	// Add double constant
	Variable* createConstant(double d, bool permanent = false);
	// Add variable
	Variable* createVariable(const char* name, Node* initialValue = 0, bool permanent = false);
	// Search for variable
	Variable* variable(const char* name);
	// Return variables
	RefListItem<Variable,bool>* variables();
	// Return constants
	RefListItem<Variable,bool>* constants();


	/*
	 * Execution
	 */
	public:
	// Execute
	double execute(bool& success);
};

#endif
