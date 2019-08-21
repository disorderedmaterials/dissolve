/*
	*** Keyword - NodeValue Triplet
	*** src/keywords/vec3nodevalue.cpp
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

#include "keywords/vec3nodevalue.h"
#include "procedure/nodes/node.h"
#include "base/lineparser.h"

// Constructors
Vec3NodeValueKeyword::Vec3NodeValueKeyword(ProcedureNode* parentNode, Vec3<double> value, Vec3Labels::LabelType labelType) : KeywordData< Vec3<NodeValue> >(KeywordBase::Vec3NodeValueData, Vec3<NodeValue>(value.x, value.y, value.z))
{
	parentNode_ = parentNode;
	labelType_ = labelType;
}

// Destructor
Vec3NodeValueKeyword::~Vec3NodeValueKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3NodeValueKeyword::minArguments()
{
	return 3;
}

// Return maximum number of arguments accepted
int Vec3NodeValueKeyword::maxArguments()
{
	return 3;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool Vec3NodeValueKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	if (!parentNode_) return false;

	if (parser.hasArg(startArg+2))
	{
		if (!data_.x.set(parser.argc(startArg), parentNode_->parametersInScope())) return false;
		if (!data_.y.set(parser.argc(startArg+1), parentNode_->parametersInScope())) return false;
		if (!data_.z.set(parser.argc(startArg+2), parentNode_->parametersInScope())) return false;

		dataHasBeenSet();

		return true;
	}

	return false;
}

// Write keyword data to specified LineParser
bool Vec3NodeValueKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %s  %s  %s\n", prefix, keyword(), data_.x.asString(true), data_.y.asString(true), data_.z.asString(true));
}

/*
 * Set
 */

// Set the value from supplied expression text
bool Vec3NodeValueKeyword::setValue(int index, const char* expressionText)
{
	if ((index < 0) || (index > 2)) return Messenger::error("Index %i out of range in Vec3NodeValueKeyword::setValue().\n", index);

	if (!data_[index].set(expressionText, parentNode_->parametersInScope())) return false;

	set_ = true;

	return true;
}

/*
 * Conversion
 */

// Return value (as Vec3<int>)
Vec3<int> Vec3NodeValueKeyword::asVec3Int()
{
	return Vec3<int>(data_.x.asInteger(), data_.y.asInteger(), data_.z.asInteger());
}

// Return value (as Vec3<NodeValue>)
Vec3<double> Vec3NodeValueKeyword::asVec3Double()
{
	return Vec3<double>(data_.x.asDouble(), data_.y.asDouble(), data_.z.asDouble());
}
