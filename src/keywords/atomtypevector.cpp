// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/atomtypevector.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"

AtomTypeVectorKeyword::AtomTypeVectorKeyword()
    : KeywordData<std::vector<std::shared_ptr<AtomType>>>(KeywordBase::AtomTypeVectorData, {})
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool AtomTypeVectorKeyword::isDataEmpty() const { return data_.empty(); }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int AtomTypeVectorKeyword::maxArguments() const { return 999; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool AtomTypeVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
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
        if (std::find(data_.begin(), data_.end(), atomType) != data_.end())
            return Messenger::error("AtomType '{}' specified in selection list twice.\n", parser.argsv(n));

        // All OK - add it to our selection list
        data_.push_back(atomType);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool AtomTypeVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over the AtomType selection list
    std::string selection;
    for (auto &at : data_)
        selection += fmt::format("  {}", at->name());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, selection))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeVectorKeyword::removeReferencesTo(std::shared_ptr<AtomType> at)
{
    auto it = std::find(data_.begin(), data_.end(), at);
    if (it != data_.end())
        data_.erase(it);
}
