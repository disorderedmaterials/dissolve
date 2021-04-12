// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/atomtypeselection.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/configuration.h"
#include "classes/coredata.h"

AtomTypeSelectionKeyword::AtomTypeSelectionKeyword(AtomTypeList &selection, const RefList<Configuration> &sourceConfigurations)
    : KeywordData<AtomTypeList &>(KeywordBase::AtomTypeSelectionData, selection), sourceConfigurations_(sourceConfigurations)
{
}

AtomTypeSelectionKeyword::~AtomTypeSelectionKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool AtomTypeSelectionKeyword::isDataEmpty() const { return data_.nItems() == 0; }

// Check AtomType selection and make sure it is consistent based on the source Configurations
void AtomTypeSelectionKeyword::checkSelection()
{
    AtomTypeList newSelection;

    // Loop over existing selection, checking for each AtomType existing in any source Configuration
    for (const AtomTypeData &atd : data_)
    {
        auto found = false;
        for (auto cfg : sourceConfigurations_)
        {
            if (cfg->usedAtomTypesList().contains(atd.atomType()))
            {
                found = true;
                break;
            }
        }

        if (found)
            newSelection.add(atd.atomType());
    }

    // Copy the new list over the old one
    data_ = newSelection;
}

// Return list of AtomTpe/bool references
AtomTypeList &AtomTypeSelectionKeyword::selection()
{
    // Update the list first, in case a Configuration has changed
    checkSelection();

    return data_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeSelectionKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int AtomTypeSelectionKeyword::maxArguments() const { return 999; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool AtomTypeSelectionKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Make sure our list is up-to-date
    checkSelection();

    // Loop over arguments (which are AtomType names) and add them to our list
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the AtomType?
        auto it = std::find_if(coreData.atomTypes().begin(), coreData.atomTypes().end(), [&parser, n](const auto atomType) {
            return DissolveSys::sameString(atomType->name(), parser.argsv(n));
        });
        if (it == coreData.atomTypes().end())
            return Messenger::error("Unrecognised AtomType '{}' found in list.\n", parser.argsv(n));
        auto atomType = *it;

        // If the AtomType is in the list already, complain
        if (data_.contains(atomType))
            return Messenger::error("AtomType '{}' specified in selection list twice.\n", parser.argsv(n));

        // All OK - add it to our selection list
        data_.add(atomType);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool AtomTypeSelectionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over the AtomType selection list
    std::string selection;
    for (auto &atd : data_)
        selection += fmt::format("  {}", atd.atomTypeName());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, selection))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeSelectionKeyword::removeReferencesTo(std::shared_ptr<AtomType> at) { data_.remove(at); }
