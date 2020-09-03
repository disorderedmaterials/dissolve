/*
    *** Keyword - NodeValue
    *** src/keywords/nodevalue.cpp
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

#include "keywords/nodevalue.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"

NodeValueKeyword::NodeValueKeyword(ProcedureNode *parentNode, NodeValue value)
    : KeywordData<NodeValue>(KeywordData::NodeValueData, value)
{
    parentNode_ = parentNode;
}

NodeValueKeyword::~NodeValueKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeValueKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int NodeValueKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeValueKeyword::read(LineParser &parser, int startArg, CoreData &coreData) { return setValue(parser.argsv(startArg)); }

// Write keyword data to specified LineParser
bool NodeValueKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.asString()))
        return false;

    return true;
}

/*
 * Set
 */

// Set the value from supplied expression text
bool NodeValueKeyword::setValue(std::string_view expressionText)
{
    if (!data_.set(expressionText, parentNode_->parametersInScope()))
        return false;

    set_ = true;

    return true;
}

/*
 * Conversion
 */

// Return value (as int)
int NodeValueKeyword::asInt() { return data_.asInteger(); }

// Return value (as double)
double NodeValueKeyword::asDouble() { return data_.asDouble(); }

/*
 * Object Management
 */
