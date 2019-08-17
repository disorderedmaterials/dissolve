/*
	*** Keyword - NodeValueEnum
	*** src/keywords/nodevalueenum.h
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

#ifndef DISSOLVE_KEYWORD_NODEVALUEENUMOPTIONS_H
#define DISSOLVE_KEYWORD_NODEVALUEENUMOPTIONS_H

#include "keywords/data.h"
#include "keywords/base.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"
#include "templates/venum.h"

// Forward Declarations
class ProcedureNode;

// Keyword with NodeValue
template <class E> class NodeValueEnumOptionsKeyword : public KeywordData< Venum<NodeValue,E> >
{
	public:
	// Constructor
	NodeValueEnumOptionsKeyword(ProcedureNode* parentNode, NodeValue value, EnumOptions<E> enumOptions) : KeywordData< Venum<NodeValue,E> >(KeywordBase::NodeValueEnumOptionsData, Venum<NodeValue,E>(value,enumOptions))
	{
		parentNode_ = parentNode;
	}
	// Destructor
	~NodeValueEnumOptionsKeyword()
	{
	}


	/*
	 * Parent Node
	 */
	private:
	// Parent ProcedureNode
	ProcedureNode* parentNode_;


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments()
	{
		return 2;
	}
	// Return maximum number of arguments accepted
	int maxArguments()
	{
		return 2;
	}
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData)
	{
		// Check that the parent node has been set
		if (!parentNode_) return Messenger::error("Parent node in NodeValueEnumOptions keyword '%s' not set. Can't read data.\n", KeywordData< Venum<NodeValue,E> >::keyword());

		// Need two args...
		if (parser.hasArg(startArg+1))
		{
			// Parse the value to start with...
			if (!KeywordData< Venum<NodeValue,E> >::data_.value().set(parser.argc(startArg), parentNode_->parametersInScope())) return false;

			// Now the enum option
			if (!KeywordData< Venum<NodeValue,E> >::data_.setEnumeration(parser.argc(startArg+1))) return false;

			return true;
		}

		return false;
	}
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix)
	{
		return parser.writeLineF("%s%s  '%s'  %s\n", prefix, KeywordData< Venum<NodeValue,E> >::keyword(), KeywordData< Venum<NodeValue,E> >::data_.value().asString(), KeywordData< Venum<NodeValue,E> >::data_.enumerationAsString());
	}


	/*
	 * Conversion
	 */
	public:
	// Return value (as int)
	int asInt()
	{
		return KeywordData< Venum<NodeValue,E> >::data_.value().asInteger();
	}
	// Return value (as double)
	double asDouble()
	{
		return KeywordData< Venum<NodeValue,E> >::data_.value().asDouble();
	}
};

#endif

