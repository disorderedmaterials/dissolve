// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/base.h"
#include "base/messenger.h"

// Static Singletons
RefList<KeywordBase> KeywordBase::allKeywords_;

KeywordBase::KeywordBase(const std::type_index typeIndex, KeywordDataType type)
    : type_(type), typeIndex_(typeIndex), set_(false)
{
    // Add ourselves to the master reference list of keywords
    allKeywords_.append(this);
}

KeywordBase::~KeywordBase()
{
    // Remove ourselves from the master reference list of keywords
    allKeywords_.remove(this);
}

// Value Keyword Data Type Keywords
std::string_view KeywordDataTypeKeywords[] = {"AtomTypeVector",
                                              "Bool",
                                              "ConfigurationRefList",
                                              "Data1DStore",
                                              "Data2DStore",
                                              "Data3DStore",
                                              "Double",
                                              "DynamicSites",
                                              "ElementVector",
                                              "EnumOptions",
                                              "Expression",
                                              "ExpressionVariableList",
                                              "FileAndFormat",
                                              "Function1D",
                                              "GeometryList",
                                              "Integer",
                                              "IsotopologueList",
                                              "IsotopologueSet",
                                              "LinkToKeyword",
                                              "Module",
                                              "ModuleGroups",
                                              "ModuleRefList",
                                              "Node",
                                              "NodeAndInteger",
                                              "NodeBranch",
                                              "NodeValue",
                                              "NodeValueEnumOptions",
                                              "NodeVector",
                                              "Procedure",
                                              "Range",
                                              "Species",
                                              "SpeciesSite",
                                              "SpeciesSiteVector",
                                              "SpeciesVector",
                                              "String",
                                              "ValueStore",
                                              "Vec3<Double>",
                                              "Vec3<Integer>",
                                              "Vec3<NodeValue>",
                                              "Vector<Integer,Double>",
                                              "Vector<Integer,String>",
                                              "Vector<String,Double>",
                                              "Vector<String,String>"};

// Return ValueType name
std::string_view KeywordBase::keywordDataType(KeywordDataType kdt) { return KeywordDataTypeKeywords[kdt]; }

/*
 * Keyword Description
 */

// Set base keyword information
void KeywordBase::setBaseInfo(std::string_view name, std::string_view description)
{
    name_ = name;
    description_ = description;
}

// Set option mask
void KeywordBase::setOptionMask(int opttionMask) { optionMask_ = opttionMask; }

// Flag that data has been set by some other means
void KeywordBase::setAsModified() { set_ = true; }

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
bool KeywordBase::isDataEmpty() const { return !set_; }

// Return whether the keyword has been set, and is not currently empty (if relevant)
bool KeywordBase::hasBeenSet() const { return set_ && !isDataEmpty(); }

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
