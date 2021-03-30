// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/atomtypereflist.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"

AtomTypeRefListKeyword::AtomTypeRefListKeyword(std::vector<std::shared_ptr<AtomType>> &targetRefList)
    : KeywordData<std::vector<std::shared_ptr<AtomType>> &>(KeywordBase::AtomTypeRefListData, targetRefList)
{
}

AtomTypeRefListKeyword::~AtomTypeRefListKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeRefListKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int AtomTypeRefListKeyword::maxArguments() const { return 999; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool AtomTypeRefListKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments (which are AtomType names) and add them to our list
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the AtomType?
        auto atomType = coreData.findAtomType(parser.argsv(n));
        if (!atomType)
            return Messenger::error("Unrecognised AtomType '{}' found in list.\n", parser.argsv(n));

        // If the AtomType is in the list already, complain
        if (std::find(data_.begin(), data_.end(), atomType) != data_.end())
            return Messenger::error("AtomType '{}' specified in list twice.\n", parser.argsv(n));

        // All OK - add it to our selection list
        data_.push_back(atomType);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool AtomTypeRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Don't write anything if there are no items in the list
    if (data_.empty())
        return true;

    // Loop over the AtomType selection list
    std::string atomTypes;
    for (auto at : data_)
        atomTypes += fmt::format("  {}", at->name());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, atomTypes))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeRefListKeyword::removeReferencesTo(std::shared_ptr<AtomType> at)
{
    data_.erase(std::remove(data_.begin(), data_.end(), at));
}
