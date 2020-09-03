/*
    *** General Node for Expression
    *** src/expression/node.h
    Copyright T. Youngs 2015-2020

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

#pragma once

#include "expression/value.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward declarations
class Expression;

// Expresison Node
class ExpressionNode : public ListItem<ExpressionNode>
{
    public:
    ExpressionNode();
    virtual ~ExpressionNode();
    // List pointers (for argument list)
    ExpressionNode *nextArgument, *prevArgument;
    // Node Types
    enum NodeType
    {
        BasicNode,
        FunctionNode,
        ValueNode,
        VariableNode,
        VariableValueNode,
        nNodeTypes
    };
    // Copy data
    void copy(ExpressionNode *source);

    /*
     * Node Type
     */
    protected:
    // Type of node
    NodeType nodeType_;
    // Pointer to parent expression
    Expression *parent_;

    public:
    // Retrieve node type
    NodeType nodeType() const;
    // Set parent
    void setParent(Expression *parent);
    // Retrieve parent
    Expression *parent() const;

    /*
     * Argument Data
     */
    protected:
    // Arguments (if any) to leaf node operation
    RefList<ExpressionNode> args_;

    public:
    // Return number of arguments currently assigned to node
    int nArgs() const;
    // Add list of arguments formas as a plain List<Node>, beginning from supplied list head
    void addListArguments(ExpressionNode *leaf);
    // Add list of arguments joined by parser, probably with list tail supplied
    void addJoinedArguments(ExpressionNode *args);
    // Add multiple arguments to node
    void addArguments(int nargs, ...);
    // Add multiple arguments to node
    void addArgument(ExpressionNode *arg);
    // Return (execute) argument specified
    bool arg(int i, ExpressionValue &result);
    // Return (execute) argument specified as a bool
    bool argb(int i);
    // Return (execute) argument specified as an integer
    int argi(int i);
    // Return (execute) argument specified as a double
    double argd(int i);
    // Return the Node corresponding to the argument, rather than executing it
    ExpressionNode *argNode(int i);
    // Set argument specified
    bool setArg(int i, ExpressionValue &result);
    // Return whether argument i was given
    bool hasArg(int i);

    /*
     * Node Character
     */
    protected:
    // Whether node is read-only
    bool readOnly_;

    public:
    // Set the readonly status of the node to true
    void setReadOnly();
    // Return the readonly status of the node
    bool readOnly() const;

    /*
     * Node Data Set / Get / Execute
     */
    public:
    // Set from value
    virtual bool set(ExpressionValue value) = 0;
    // Get reduced value of node
    virtual bool execute(ExpressionValue &result) = 0;
    // Print layout of current node
    virtual void nodePrint(int offset, std::string_view prefix = "") = 0;
    // Reset node
    virtual bool initialise() = 0;
};
