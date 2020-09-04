/*
    *** General Node for Expression
    *** src/expression/node.cpp
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

#include "expression/node.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "templates/reflist.h"
#include <cctype>
#include <stdarg.h>
#include <string.h>

// Local constant
const int MAXNODEARGS = 10;

ExpressionNode::ExpressionNode() : ListItem<ExpressionNode>()
{
    // Private variables
    readOnly_ = true;
    parent_ = nullptr;
    nextArgument = nullptr;
    prevArgument = nullptr;
    nodeType_ = ExpressionNode::BasicNode;
}

ExpressionNode::~ExpressionNode() {}

// Copy data
void ExpressionNode::copy(ExpressionNode *source)
{
    nodeType_ = source->nodeType_;
    parent_ = source->parent_;
    args_ = source->args_;
    readOnly_ = source->readOnly_;
}

// Retrieve node type
ExpressionNode::NodeType ExpressionNode::nodeType() const { return nodeType_; }

// Set parent
void ExpressionNode::setParent(Expression *parent) { parent_ = parent; }

// Retrieve parent
Expression *ExpressionNode::parent() const { return parent_; }

// Set the readonly status of the variable to true
void ExpressionNode::setReadOnly() { readOnly_ = true; }

// Return readonly status
bool ExpressionNode::readOnly() const { return readOnly_; }

// Return number of arguments currently assigned to node
int ExpressionNode::nArgs() const { return args_.nItems(); }

// Set argument specified
bool ExpressionNode::setArg(int i, ExpressionValue &result)
{
    if ((i < 0) || (i >= args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::setArg : Argument index {} is out of range.\n", i);
        return false;
    }

    // Check read-only attribute of argument
    if (args_[i]->item()->readOnly())
    {
        args_[i]->item()->nodePrint(0);
        return false;
    }

    return args_[i]->item()->set(result);
}

// Return whether argument i was given
bool ExpressionNode::hasArg(int i) { return (i < args_.nItems()); }

// Add list of arguments formas as a plain List<Node>, beginning from supplied list head
void ExpressionNode::addListArguments(ExpressionNode *leaf)
{
    for (auto *node = leaf; node != nullptr; node = node->next())
        args_.append(node);
}

// Add list of arguments formed as a linked Node list
void ExpressionNode::addJoinedArguments(ExpressionNode *lastleaf)
{
    /*
     * The supplied leaf may be a single node, or it may be a list of nodes beginning at the *last* node (this is the case
     * if Joined by the parser) Therefore, must walk backwards through the list first to get to the head...
     */
    ExpressionNode *first;
    for (first = lastleaf; first != nullptr; first = first->prevArgument)
        if (first->prevArgument == nullptr)
            break;
    for (auto *node = first; node != nullptr; node = node->nextArgument)
        args_.append(node);
}

// Add multiple arguments to node
void ExpressionNode::addArguments(int nargs, ...)
{
    // Create variable argument parser
    va_list vars;
    va_start(vars, nargs);

    // Add arguments in the order they were provided
    for (int n = 0; n < nargs; n++)
        addArgument(va_arg(vars, ExpressionNode *));
    va_end(vars);
}

// Add multiple arguments to node
void ExpressionNode::addArgument(ExpressionNode *arg) { args_.append(arg); }

// Return (execute) argument specified
bool ExpressionNode::arg(int i, ExpressionValue &result)
{
    if ((i < 0) || (i >= args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::arg : Argument index {} is out of range.\n", i);
        return false;
    }
    return args_[i]->item()->execute(result);
}

// Return (execute) argument specified as a bool
bool ExpressionNode::argb(int i)
{
    if ((i < 0) || (i >= args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::argb : Argument index {} is out of range.\n", i);
        return false;
    }

    ExpressionValue argValue;
    if (!args_[i]->item()->execute(argValue))
        Messenger::printVerbose("Couldn't retrieve argument {}.\n", i + 1);

    return (argValue.isInteger() ? argValue.asInteger() > 0 : argValue.asDouble() > 0.0);
}

// Return (execute) argument specified as an integer
int ExpressionNode::argi(int i)
{
    if ((i < 0) || (i >= args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::argi : Argument index {} is out of range.\n", i);
        return false;
    }

    ExpressionValue argValue;
    if (!args_[i]->item()->execute(argValue))
        Messenger::printVerbose("Couldn't retrieve argument {}.\n", i + 1);

    return argValue.asInteger();
}

// Return (execute) argument specified as a double
double ExpressionNode::argd(int i)
{
    if ((i < 0) || (i >= args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::argd : Argument index {} is out of range.\n", i);
        return false;
    }

    ExpressionValue argValue;
    if (!args_[i]->item()->execute(argValue))
        Messenger::printVerbose("Couldn't retrieve argument {}.\n", i + 1);

    return argValue.asDouble();
}

// Return the Node corresponding to the argument, rather than executing it
ExpressionNode *ExpressionNode::argNode(int i)
{
    if ((i < 0) || (i > args_.nItems()))
    {
        Messenger::printVerbose("ExpressionNode::argNode : Argument index {} is out of range for returning the "
                                "argument node.\n",
                                i);
        return nullptr;
    }
    return args_[i]->item();
}
