/*
	*** Keyword Base Class
	*** src/module/keywordbase.cpp
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

#include "keywords/base.h"
#include "base/messenger.h"

// Constructor
KeywordBase::KeywordBase(KeywordDataType type) : ListItem<KeywordBase>(), type_(type)
{
	set_ = false;
}

// Destructor
KeywordBase::~KeywordBase()
{
}

// Value Keyword Data Type Keywords
const char* KeywordDataTypeKeywords[] = { "AtomTypeSelection", "Bool", "BroadeningFunction", "CharString", "Complex", "Data1DStore", "Data2DStore", "Data3DStore", "Double", "EnumOptions", "EnumString", "FileAndFormat", "Integer", "IsotopologueList", "ModuleGroups", "ModuleReferenceList", "PairBroadeningFunction", "Procedure", "SpeciesReferenceList", "SpeciesSite", "SpeciesSiteReferenceList", "Vec3<Double>", "Vec3<Integer>", "WindowFunction" };

// Return ValueType name
const char* KeywordBase::keywordDataType(KeywordDataType kdt)
{
	return KeywordDataTypeKeywords[kdt];
}


/*
 * Keyword Description
 */

// Set name, description, and item flags
void KeywordBase::set(const char* keyword, const char* description, const char* arguments, int genericItemFlags)
{
	keyword_ = keyword;
	arguments_ = arguments;
	description_ = description;
	genericItemFlags_ = genericItemFlags;
}

// Return data type stored by keyword
KeywordBase::KeywordDataType KeywordBase::type()
{
	return type_;
}

// Return keyword name
const char* KeywordBase::keyword()
{
	return keyword_.get();
}

// Return keyword description
const char* KeywordBase::description()
{
	return description_.get();
}

// Return flags to apply if reinstated as a GenericListItem (i.e. in a Module)
int KeywordBase::genericItemFlags()
{
	return genericItemFlags_;
}

// Return whether the data has ever been set
bool KeywordBase::isSet()
{
	return set_;
}

/*
 * Conversion
 */

// Return value (as bool)
bool KeywordBase::asBool()
{
	Messenger::warn("No suitable conversion to bool from KeywordDataType %i (%s) exists. Returning 'false'.\n", type_, KeywordBase::keywordDataType(type_));
	return false;
}

// Return value (as int)
int KeywordBase::asInt()
{
	Messenger::warn("No suitable conversion to int from KeywordDataType %i (%s) exists. Returning '0'.\n", type_, KeywordBase::keywordDataType(type_));
	return 0;
}

// Return value (as double)
double KeywordBase::asDouble()
{
	Messenger::warn("No suitable conversion to double from KeywordDataType %i (%s) exists. Returning '0.0'.\n", type_, KeywordBase::keywordDataType(type_));
	return 0.0;
}

// Return value (as string)
const char* KeywordBase::asString()
{
	Messenger::warn("No suitable conversion to string from KeywordDataType %i (%s) exists. Returning 'NULL'.\n", type_, KeywordBase::keywordDataType(type_));
	return "NULL";
}

// Return value as Vec3<int>
Vec3<int> KeywordBase::asVec3Int()
{
	Messenger::warn("No suitable conversion to Vec3<int> from KeywordDataType %i (%s) exists. Returning '(0,0,0)'.\n", type_, KeywordBase::keywordDataType(type_));
	return Vec3<int>(0,0,0);
}

// Return value as Vec3<double>
Vec3<double> KeywordBase::asVec3Double()
{
	Messenger::warn("No suitable conversion to Vec3<double> from KeywordDataType %i (%s) exists. Returning '(0.0,0.0,0.0)'.\n", type_, KeywordBase::keywordDataType(type_));
	return Vec3<double>(0.0,0.0,0.0);
}
