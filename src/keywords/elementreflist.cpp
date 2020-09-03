/*
    *** Keyword - Element RefList
    *** src/keywords/elementreflist.cpp
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

#include "keywords/elementreflist.h"
#include "base/lineparser.h"
#include "data/elements.h"

ElementRefListKeyword::ElementRefListKeyword(RefList<Element> &targetRefList)
    : KeywordData<RefList<Element> &>(KeywordBase::ElementRefListData, targetRefList)
{
}

ElementRefListKeyword::~ElementRefListKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ElementRefListKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ElementRefListKeyword::maxArguments() const { return 999; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ElementRefListKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Loop over arguments (which are Element names) and add them to our list
    for (int n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the Element?
        Element *el = Elements::elementPointer(parser.argsv(n));
        if (!el)
            return Messenger::error("Unrecognised Element '{}' found in list.\n", parser.argsv(n));

        // If the Element is in the list already, complain
        if (data_.contains(el))
            return Messenger::error("Element '{}' specified in list twice.\n", parser.argsv(n));

        // All OK - add it to our selection list
        data_.append(el);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ElementRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Don't write anything if there are no items in the list
    if (data_.nItems() == 0)
        return true;

    // Loop over the Element list
    std::string elements;
    for (auto el : data_)
        elements += fmt::format("  {}", el->symbol());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, elements))
        return false;

    return true;
}
