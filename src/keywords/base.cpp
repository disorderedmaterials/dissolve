// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/base.h"
#include "base/messenger.h"

KeywordBase::KeywordBase(const std::type_index typeIndex) : typeIndex_(typeIndex) {}

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

// Return keyword name
std::string_view KeywordBase::name() const { return name_; }

// Return keyword description
std::string_view KeywordBase::description() const { return description_; }

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
 * GUI Signalling
 */

// Set signals to be emitted (via Qt) when editing this keyword in the GUI
void KeywordBase::setEditSignals(Flags<KeywordBase::KeywordSignal> editSignals) { signals_ += editSignals; }

// Return signals to be emitted (via Qt) when editing this keyword in the GUI
Flags<KeywordBase::KeywordSignal> KeywordBase::editSignals() const { return signals_; }

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
void KeywordBase::removeReferencesTo(NodeRef node)
{
    // Default action - ignore
}
