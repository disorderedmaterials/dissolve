/*
    *** Keyword - AtomType RefList
    *** src/keywords/atomtypereflist.cpp
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
bool AtomTypeRefListKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Loop over arguments (which are AtomType names) and add them to our list
    for (int n = startArg; n < parser.nArgs(); ++n)
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
bool AtomTypeRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
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
