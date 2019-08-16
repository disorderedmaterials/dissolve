/*
	*** Keyword Base Class
	*** src/keywords/base.cpp
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

// Static Singletons
RefList<KeywordBase> KeywordBase::allKeywords_;

// Constructor
KeywordBase::KeywordBase(KeywordDataType type) : ListItem<KeywordBase>(), type_(type)
{
	set_ = false;

	// Add ourselves to the master reference list of keywords
	allKeywords_.append(this);
}

// Destructor
KeywordBase::~KeywordBase()
{
	// Remove ourselves from the master reference list of keywords
	allKeywords_.remove(this);
}

// Value Keyword Data Type Keywords
const char* KeywordDataTypeKeywords[] = { "AtomTypeSelection", "Bool", "BroadeningFunction", "CharString", "Complex", "Data1DStore", "Data2DStore", "Data3DStore", "Double", "EnumOptions", "EnumString", "FileAndFormat", "Integer", "IsotopologueList", "ModuleGroups", "ModuleReferenceList", "NodeValue", "PairBroadeningFunction", "Procedure", "SpeciesReferenceList", "SpeciesSite", "SpeciesSiteReferenceList", "Vec3<Double>", "Vec3<Integer>", "WindowFunction" };

// Return ValueType name
const char* KeywordBase::keywordDataType(KeywordDataType kdt)
{
	return KeywordDataTypeKeywords[kdt];
}

/*
 * Keyword Description
 */

// Set name, description, arguments, and option mask
void KeywordBase::set(const char* keyword, const char* description, const char* arguments, int optionMask)
{
	keyword_ = keyword;
	arguments_ = arguments;
	description_ = description;
	optionMask_ = optionMask;
}

// Return data type stored by keyword
KeywordBase::KeywordDataType KeywordBase::type()
{
	return type_;
}

// Return name of data type stored by keyword
const char* KeywordBase::typeName() const
{
	return KeywordDataTypeKeywords[type_];
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

// Return whether to save keyword value in the restart file
bool KeywordBase::saveInRestart() const
{
	return (optionMask_&KeywordBase::InRestartFileOption);
}

// Return whether the data has ever been set
bool KeywordBase::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Check number of arguments provided to keyword
bool KeywordBase::validNArgs(int nArgsProvided)
{
	if (nArgsProvided < minArguments())
	{
		Messenger::error("Not enough arguments given to %s keyword '%s'.\n", typeName(), keyword());
		return false;
	}
	if ((maxArguments() >= 0) && (nArgsProvided > maxArguments()))
	{
		Messenger::error("Too many arguments given to %s keyword '%s'.\n", typeName(), keyword());
		return false;
	}

	return true;
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

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void KeywordBase::removeReferencesTo(AtomType* at)
{
	// Default action - ignore
}

// Prune any references to the supplied Configuration in the contained data
void KeywordBase::removeReferencesTo(Configuration* cfg)
{
	// Default action - ignore
}

// Prune any references to the supplied Isotopologue in the contained data
void KeywordBase::removeReferencesTo(Isotopologue* iso)
{
	// Default action - ignore
}

// Prune any references to the supplied Module in the contained data
void KeywordBase::removeReferencesTo(Module* module)
{
	// Default action - ignore
}

// Prune any references to the supplied Species in the contained data
void KeywordBase::removeReferencesTo(Species* sp)
{
	// Default action - ignore
}

// Prune any references to the supplied SpeciesSite in the contained data
void KeywordBase::removeReferencesTo(SpeciesSite* spSite)
{
	// Default action - ignore
}
