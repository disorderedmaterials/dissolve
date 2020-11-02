// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    for (auto n = startArg; n < parser.nArgs(); ++n)
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
