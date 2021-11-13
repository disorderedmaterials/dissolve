// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/base.h"
#include "base/messenger.h"

// Static Singletons
RefList<KeywordBase> KeywordBase::allKeywords_;

KeywordBase::KeywordBase(const std::type_index typeIndex) : typeIndex_(typeIndex), set_(false)
{
    // Add ourselves to the master reference list of keywords
    allKeywords_.append(this);
}

KeywordBase::~KeywordBase()
{
    // Remove ourselves from the master reference list of keywords
    allKeywords_.remove(this);
}

/*
 * Keyword Description
 */

// Set base keyword information
void KeywordBase::setBaseInfo(std::string_view name, std::string_view description)
{
    name_ = name;
    description_ = description;
}

// Return typeindex for the keyword
const std::type_index KeywordBase::typeIndex() const { return typeIndex_; }

// Set option mask
void KeywordBase::setOptionMask(int optionMask) { optionMask_ = optionMask; }

// Flag that data has been set by some other means
void KeywordBase::setAsModified() { set_ = true; }

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

// Return minimum number of arguments accepted
int KeywordBase::minArguments() const { return 1; }

// Return maximum number of arguments accepted
std::optional<int> KeywordBase::maxArguments() const { return 1; }

// Check number of arguments provided to keyword
bool KeywordBase::validNArgs(int nArgsProvided) const
{
    if (nArgsProvided < minArguments())
        return Messenger::error("Not enough arguments given to keyword '{}'.\n", name());
    if (maxArguments() && (nArgsProvided > maxArguments().value()))
        return Messenger::error("Too many arguments given to keyword '{}'.\n", name());

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
