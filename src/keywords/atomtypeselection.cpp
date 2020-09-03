/*
    *** Keyword - AtomType Selection
    *** src/keywords/atomtypeselection.cpp
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
bool AtomTypeSelectionKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Make sure our list is up-to-date
    checkSelection();

    // Loop over arguments (which are AtomType names) and add them to our list
    for (int n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the AtomType?
        auto it =
            std::find_if(coreData.constAtomTypes().begin(), coreData.constAtomTypes().end(), [&parser, n](const auto atomType) {
                return DissolveSys::sameString(atomType->name(), parser.argsv(n));
            });
        if (it == coreData.constAtomTypes().end())
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
bool AtomTypeSelectionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
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
