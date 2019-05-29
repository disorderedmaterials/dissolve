/*
	*** General Node for Expression
	*** src/expression/node.cpp
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

#include "expression/node.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "templates/reflist.h"
#include <stdarg.h>
#include <string.h>
#include <cctype>

// Local constant
const int MAXNODEARGS = 10;

// Constructors
ExpressionNode::ExpressionNode() : ListItem<ExpressionNode>()
{
	// Private variables
	returnsNumber_ = false;
	readOnly_ = true;
	parent_ = NULL;
	nextArgument = NULL;
	prevArgument = NULL;
	nodeType_ = ExpressionNode::BasicNode;
}

// Destructor
ExpressionNode::~ExpressionNode()
{
}

// Copy data
void ExpressionNode::copy(ExpressionNode* source)
{
	nodeType_ = source->nodeType_;
	parent_ = source->parent_;
	args_ = source->args_;
	returnsNumber_ = source->returnsNumber_;
	readOnly_ = source->readOnly_;
}

// Retrieve node type
ExpressionNode::NodeType ExpressionNode::nodeType() const
{
	return nodeType_;
}

// Set parent 
void ExpressionNode::setParent(Expression* parent)
{
	parent_ = parent;
}

// Retrieve parent
Expression* ExpressionNode::parent() const
{
	return parent_;
}

// Set whether node returns a number
void ExpressionNode::setReturnsNumber(bool b)
{
	returnsNumber_ = b;
}

// Return whether node returns a number
bool ExpressionNode::returnsNumber()
{
	return returnsNumber_;
}

// Set the readonly status of the variable to true
void ExpressionNode::setReadOnly()
{
	readOnly_ = true;
}

// Return readonly status
bool ExpressionNode::readOnly() const
{
	return readOnly_;
}

// Return number of arguments currently assigned to node
int ExpressionNode::nArgs() const
{
	return args_.nItems();
}

// Return whether nth argument returns a number
bool ExpressionNode::isArgNumeric(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::argType : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	return args_[i]->item->returnsNumber();
}

// Set argument specified
bool ExpressionNode::setArg(int i, double& rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::setArg : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	// Check readonly attribute of argument
	if (args_[i]->item->readOnly())
	{
		args_[i]->item->nodePrint(0);
		//printf("Argument %i is read-only and can't be set.\n", i);
		return false;
	}
	return args_[i]->item->set(rv);
}

// Return whether argument i was given
bool ExpressionNode::hasArg(int i)
{
	return (i < args_.nItems());
}

// Add list of arguments formas as a plain List<Node>, beginning from supplied list head
void ExpressionNode::addListArguments(ExpressionNode* leaf)
{
	for (ExpressionNode* node = leaf; node != NULL; node = node->next) args_.add(node);
}

// Add list of arguments formed as a linked Node list
void ExpressionNode::addJoinedArguments(ExpressionNode* lastleaf)
{
	/*
	 * The supplied leaf may be a single node, or it may be a list of nodes beginning at the *last* node (this is the case if Joined by the parser)
	 * Therefore, must walk backwards through the list first to get to the head...
	 */
	ExpressionNode* first;
	for (first = lastleaf; first != NULL; first = first->prevArgument) if (first->prevArgument == NULL) break;
	for (ExpressionNode* node = first; node != NULL; node = node->nextArgument) args_.add(node);
}

// Add multiple arguments to node
void ExpressionNode::addArguments(int nargs, ...)
{
	// Create variable argument parser
	va_list vars;
	va_start(vars,nargs);
	// Add arguments in the order they were provided
	for (int n=0; n<nargs; n++) addArgument(va_arg(vars, ExpressionNode*));
	va_end(vars);
	//Messenger::print(Messenger::Parse,"Node %p now has %i arguments.\n", this, args_.nItems());
}

// Add multiple arguments to node
void ExpressionNode::addArgument(ExpressionNode* arg)
{
	args_.add(arg);
}

// Check validity of supplied arguments
bool ExpressionNode::checkArguments(const char* arglist, const char* funcname)
{
	//msg.enter("ExpressionNode::checkArguments");
	//Messenger::print(Messenger::Parse, "Checking the %i argument(s) given to function '%s'...\n", args_.nItems(), funcname);
	const char* c = NULL, *altargs = arglist;
// 	Messenger::print(Messenger::Parse, "...argument list is [%s]\n", altargs);
	char upc;
	int count = 0, ngroup = -1, repeat = 0;
	bool optional = false, requireVar = false, result, cluster = false, reset = true;
	// If the argument list begins with '_', arguments will have already been checked and added elsewhere...
	if (*altargs == '_')
	{
// 		msg.exit("ExpressionNode::checkArguments");
		return true;
	}
	// Search for an alternative set of arguments
	result = true;
	do
	{
		if (reset)
		{
			c = altargs;
			if (*c == '|') ++c;
			altargs = strchr(c, '|');
			repeat = 0;
			cluster = false;
			count = 0;
			reset = false;
		}
		if (*c == '\0') break;
		upc = *c;
			if (*c == '|')
			{
				// This is the start of a new set of argument specifiers - does the current set of arguments 'fit'?
				if (args_.nItems() != count)
				{
// 					printf("Number of arguments (%i) doesn't match number in this set (%i) - next!\n", args_.nItems(), count);
					reset = true;
					continue;
				}
// 				msg.exit("ExpressionNode::checkArguments");
				return true;
			}
		// Retain previous information if this is a repeat, but make it an optional argument
		if (*c == '*') optional = true;
		else if (repeat == 0)
		{
			// Reset modifier values
			requireVar = false;
			repeat = 1;
			// Find next alpha character (and accompanying modifiers)
			while (!isalpha(*c) && (*c != '|') && (*c != '\0') )
			{
				switch (*c)
				{
					// Require variable
					case ('^'):	
						requireVar = true; break;
					// Clustering
					case ('['):	cluster = true; ngroup = 0; break;
					case (']'):	cluster = false; ngroup = -1; break;
					// Require array
					case ('2'):
					case ('3'):
					case ('4'):
					case ('5'):
					case ('6'):
					case ('7'):
					case ('8'):
					case ('9'):	repeat = *c - '0'; break;
					default:
						Messenger::printVerbose("Error: Bad character (%c) in command arguments\n", *c);
						break;
				}
				c++;
			}
			if (*c == '|')
			{
				// This is the start of a new set of argument specifiers - does the current set of arguments 'fit'?
				if (args_.nItems() != count)
				{
					Messenger::printVerbose("Error: Number of arguments (%i) doesn't match number in this set (%i) - next!\n", args_.nItems(), count);
					reset = true;
					continue;
				}
// 				msg.exit("ExpressionNode::checkArguments");
				return true;
			}
			// Convert character to upper case if necessary
			if ((*c > 96) && (*c < 123))
			{
				upc = *c - 32;
				optional = true;
			}
			else
			{
				upc = *c;
				optional = false;
			}
		}
		if (*c == '\0') break;
// 		Messenger::print(Messenger::Parse,"...next/current argument token is '%c', opt=%s, reqvar=%s, repeat=%i, ngroup=%i\n", *c, optional ? "true" : "false", requirevar ? "true" : "false", repeat, ngroup);
		// If we have gone over the number of arguments provided, is this an optional argument?
		if (count >= args_.nItems())
		{
			if (!optional)
			{
				// If an alternative argument list is present, check this before we fail...
				if (altargs != NULL) { reset = true; continue; }
				Messenger::printVerbose("Error: The function '%s' requires argument %i.\n", funcname, count+1);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, Command::data[function_].argText);
// 				msg.exit("ExpressionNode::checkArguments");
				return false;
			}
			else if (cluster && (ngroup != 0))
			{
				Messenger::printVerbose("Error: The optional argument %i to function '%s' is part of a group and must be specified.\n", count+1, funcname);
// 				printf("       Command syntax is '%s(%s)'.\n", funcname, arglist);
// 				msg.exit("ExpressionNode::checkArguments");
				return false;
			}
			else
			{
// 				msg.exit("ExpressionNode::checkArguments");
				return true;
			}
		}

		// Check argument type
		result = true;
		switch (upc)
		{
			// Number
			case ('N'):
				if (!isArgNumeric(count))
				{
					if (altargs != NULL) { reset = true; continue; }
					Messenger::printVerbose("Error: Argument %i to function '%s' must be a number.\n", count+1, funcname);
					result = false;
				}
				break;
		}

		// Was this argument requested to be a modifiable variable value?
		if (requireVar && argNode(count)->readOnly())
		{
			Messenger::printVerbose("Error: Argument %i to function '%s' must be a variable and not a constant.\n", count+1, funcname);
			result = false;
		}

		// Check for failure
		if (!result) break;
		if ((upc != '*') && (repeat == 1)) c++;
		if (cluster) ngroup++;
		repeat--;
		count++;
	} while (*c != '\0');
	// End of the argument specification - do we still have arguments left over in the command?
	if (result && (args_.nItems() > count))
	{
		Messenger::printVerbose("Error: %i extra arguments given to function '%s'.\n", args_.nItems()-count, funcname);
// 		msg.exit("ExpressionNode::checkArguments");
		return false;
	}
// 	msg.exit("ExpressionNode::checkArguments");
	return result;
}

// Return (execute) argument specified
bool ExpressionNode::arg(int i, double& rv)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::arg : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	return args_[i]->item->execute(rv);
}

// Return (execute) argument specified as a bool
bool ExpressionNode::argb(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::argb : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double rv;
	bool result;
	if (!args_[i]->item->execute(rv)) Messenger::printVerbose("Couldn't retrieve argument %i.\n", i+1);
	result = (rv > 0);
	return result;
}

// Return (execute) argument specified as an integer
int ExpressionNode::argi(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::argi : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double rv;
	int result = 0;
	if (!args_[i]->item->execute(rv)) Messenger::printVerbose("Couldn't retrieve argument %i.\n", i+1);
	result = (int) rv;
	return result;
}

// Return (execute) argument specified as a double
double ExpressionNode::argd(int i)
{
	if ((i < 0) || (i >= args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::argd : Argument index %i is out of range (node = %p).\n", i, this);
		return false;
	}
	double result = 0.0;
	if (!args_[i]->item->execute(result)) Messenger::printVerbose("Couldn't retrieve argument %i.\n", i+1);
	return result;
}

// Return the Node corresponding to the argument, rather than executing it
ExpressionNode* ExpressionNode::argNode(int i)
{
	if ((i < 0) || (i > args_.nItems()))
	{
		Messenger::printVerbose("ExpressionNode::argNode : Argument index %i is out of range for returning the argument node (node = %p).\n", i, this);
		return NULL;
	}
	return args_[i]->item;
}

