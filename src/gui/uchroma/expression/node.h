/*
	*** General Node for Expression
	*** src/gui/uchroma/expression/node.h
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

#ifndef DISSOLVE_UCHROMANODE_H
#define DISSOLVE_UCHROMANODE_H

#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward declarations
class Expression;

// Tree Node
class Node : public ListItem<Node>
{
	public:
	// Constructor / Destructor
	Node();
	virtual ~Node();
	// List pointers (for argument list)
	Node* nextArgument, *prevArgument;
	// Node Types
	enum NodeType { BasicNode, FuncNode, VarNode, VarWrapperNode, nNodeTypes };
	// Copy data
	void copy(Node* source);


	/*
	// Node Type
	*/
	protected:
	// Type of node
	NodeType nodeType_;
	// Pointer to parent expression
	Expression* parent_;

	public:
	// Retrieve node type
	NodeType nodeType() const;
	// Set parent 
	void setParent(Expression* parent);
	// Retrieve parent
	Expression* parent() const;


	/*
	 * Argument Data
	 */
	protected:
	// Arguments (if any) to leaf node operation
	RefList<Node,int> args_;

	public:
	// Return number of arguments currently assigned to node
	int nArgs() const;
	// Return if nth argument is numeric
	bool isArgNumeric(int i);
	// Add list of arguments formas as a plain List<Node>, beginning from supplied list head
	void addListArguments(Node* leaf);
	// Add list of arguments joined by parser, probably with list tail supplied
	void addJoinedArguments(Node* args);
	// Add multiple arguments to node
	void addArguments(int nargs, ...);
	// Add multiple arguments to node
	void addArgument(Node* arg);
	// Check arguments stored in argument list
	bool checkArguments(const char* arglist, const char* funcname);
	// Return (execute) argument specified
	bool arg(int i, double& rv);
	// Return (execute) argument specified as a bool
	bool argb(int i);
	// Return (execute) argument specified as an integer
	int argi(int i);
	// Return (execute) argument specified as a double
	double argd(int i);
	// Return the Node corresponding to the argument, rather than executing it
	Node* argNode(int i);
	// Set argument specified
	bool setArg(int i, double& rv);
	// Return whether argument i was given
	bool hasArg(int i);


	/*
	 * Node Character
	 */
	protected:
	// Whether node returns a number
	bool returnsNumber_;
	// Whether node is read-only
	bool readOnly_;

	public:
	// Set whether node returns a number
	void setReturnsNumber(bool b);
	// Return whether node returns a number
	bool returnsNumber();
	// Set the readonly status of the node to true
	void setReadOnly();
	// Return the readonly status of the node
	bool readOnly() const;


	/*
	 * Node Data Set / Get / Execute
	 */
	public:
	// Set from value
	virtual bool set(double rv) = 0;
	// Get reduced value of node
	virtual bool execute(double& rv) = 0;
	// Print layout of current node
	virtual void nodePrint(int offset, const char* prefix = "") = 0;
	// Reset node
	virtual bool initialise() = 0;
};

#endif
