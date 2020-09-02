/*
    *** Keyword Base Class
    *** src/keywords/base.cpp
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

#include "keywords/base.h"
#include "base/messenger.h"

// Static Singletons
RefList<KeywordBase> KeywordBase::allKeywords_;

KeywordBase::KeywordBase(KeywordDataType type) : ListItem<KeywordBase>(), type_(type)
{
    set_ = false;

    // Add ourselves to the master reference list of keywords
    allKeywords_.append(this);
}

KeywordBase::~KeywordBase()
{
    // Remove ourselves from the master reference list of keywords
    allKeywords_.remove(this);
}

// Value Keyword Data Type Keywords
std::string_view KeywordDataTypeKeywords[] = {"AtomTypeRefList",
                                              "AtomTypeSelection",
                                              "Bool",
                                              "BroadeningFunction",
                                              "Data1DStore",
                                              "Data2DStore",
                                              "Data3DStore",
                                              "Double",
                                              "DynamicSites",
                                              "ElementRefList",
                                              "EnumOptions",
                                              "Expression",
                                              "ExpressionVariableList",
                                              "FileAndFormat",
                                              "GeometryList",
                                              "Integer",
                                              "IsotopologueCollection",
                                              "IsotopologueList",
                                              "LinkToKeyword",
                                              "Module",
                                              "ModuleGroups",
                                              "ModuleRefList",
                                              "Node",
                                              "NodeAndInteger",
                                              "NodeArray",
                                              "NodeBranch",
                                              "NodeRefList",
                                              "NodeValue",
                                              "NodeValueEnumOptions",
                                              "PairBroadeningFunction",
                                              "Procedure",
                                              "Range",
                                              "Species",
                                              "SpeciesRefList",
                                              "SpeciesSite",
                                              "SpeciesSiteRefList",
                                              "String",
                                              "Vec3<Double>",
                                              "Vec3<Integer>",
                                              "Vec3<NodeValue>",
                                              "Vector<Integer,Double>",
                                              "Vector<Integer,String>",
                                              "WindowFunction"};

// Return ValueType name
std::string_view KeywordBase::keywordDataType(KeywordDataType kdt) { return KeywordDataTypeKeywords[kdt]; }

/*
 * Base Pointer Return
 */

// Return base pointer for this (may be overloaded to provide access to other KeywordBase instance)
KeywordBase *KeywordBase::base() { return this; }

/*
 * Keyword Description
 */

// Set name, description, arguments, and option mask
void KeywordBase::set(std::string_view name, std::string_view description, std::string_view arguments, int optionMask)
{
    name_ = name;
    arguments_ = arguments;
    description_ = description;
    optionMask_ = optionMask;
}

// Return whether data has been set
bool KeywordBase::isSet() const { return set_; }

// Flag that data has been set by some other means
void KeywordBase::hasBeenSet() { set_ = true; }

// Return data type stored by keyword
KeywordBase::KeywordDataType KeywordBase::type() const { return type_; }

// Return name of data type stored by keyword
std::string_view KeywordBase::typeName() const { return KeywordDataTypeKeywords[type_]; }

// Return keyword name
std::string_view KeywordBase::name() const { return name_; }

// Return keyword description
std::string_view KeywordBase::description() const { return description_; }

// Return keyword option mask
int KeywordBase::optionMask() const { return optionMask_; }

// Return whether specified option is set
bool KeywordBase::isOptionSet(KeywordOption opt) const { return (optionMask_ & opt); }

// Return whether the data has ever been set
bool KeywordBase::isDataEmpty() const { return set_; }

/*
 * Arguments
 */

// Check number of arguments provided to keyword
bool KeywordBase::validNArgs(int nArgsProvided) const
{
    if (nArgsProvided < minArguments())
    {
        Messenger::error("Not enough arguments given to {} keyword '{}'.\n", typeName(), name());
        return false;
    }
    if ((maxArguments() >= 0) && (nArgsProvided > maxArguments()))
    {
        Messenger::error("Too many arguments given to {} keyword '{}'.\n", typeName(), name());
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
    Messenger::warn("No suitable conversion to bool from KeywordDataType {} ({}) exists. Returning 'false'.\n", type_,
                    KeywordBase::keywordDataType(type_));
    return false;
}

// Return value (as int)
int KeywordBase::asInt()
{
    Messenger::warn("No suitable conversion to int from KeywordDataType {} ({}) exists. Returning '0'.\n", type_,
                    KeywordBase::keywordDataType(type_));
    return 0;
}

// Return value (as double)
double KeywordBase::asDouble()
{
    Messenger::warn("No suitable conversion to double from KeywordDataType {} ({}) exists. Returning '0.0'.\n", type_,
                    KeywordBase::keywordDataType(type_));
    return 0.0;
}

// Return value (as string)
std::string KeywordBase::asString()
{
    Messenger::warn("No suitable conversion to string from KeywordDataType {} ({}) exists. Returning 'NULL'.\n", type_,
                    KeywordBase::keywordDataType(type_));
    return "NULL";
}

// Return value as Vec3<int>
Vec3<int> KeywordBase::asVec3Int()
{
    Messenger::warn("No suitable conversion to Vec3<int> from KeywordDataType {} ({}) exists. Returning '(0,0,0)'.\n", type_,
                    KeywordBase::keywordDataType(type_));
    return Vec3<int>(0, 0, 0);
}

// Return value as Vec3<double>
Vec3<double> KeywordBase::asVec3Double()
{
    Messenger::warn("No suitable conversion to Vec3<double> from KeywordDataType {} ({}) exists. Returning '(0.0,0.0,0.0)'.\n",
                    type_, KeywordBase::keywordDataType(type_));
    return Vec3<double>(0.0, 0.0, 0.0);
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void KeywordBase::removeReferencesTo(std::shared_ptr<AtomType> at)
{
    // Default action - ignore
}

// Prune any references to the supplied Configuration in the contained data
void KeywordBase::removeReferencesTo(Configuration *cfg)
{
    // Default action - ignore
}

// Prune any references to the supplied Isotopologue in the contained data
void KeywordBase::removeReferencesTo(Isotopologue *iso)
{
    // Default action - ignore
}

// Prune any references to the supplied Module in the contained data
void KeywordBase::removeReferencesTo(Module *module)
{
    // Default action - ignore
}

// Prune any references to the supplied Species in the contained data
void KeywordBase::removeReferencesTo(Species *sp)
{
    // Default action - ignore
}

// Prune any references to the supplied SpeciesSite in the contained data
void KeywordBase::removeReferencesTo(SpeciesSite *spSite)
{
    // Default action - ignore
}

// Prune any references to the supplied ProcedureNode in the contained data
void KeywordBase::removeReferencesTo(ProcedureNode *node)
{
    // Default action - ignore
}
