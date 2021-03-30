// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/elementreflist.h"
#include "base/lineparser.h"
#include "data/elements.h"

ElementVectorKeyword::ElementVectorKeyword(std::vector<Elements::Element> &targetData)
    : KeywordData<std::vector<Elements::Element> &>(KeywordBase::ElementVectorData, targetData)
{
}

ElementVectorKeyword::~ElementVectorKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ElementVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ElementVectorKeyword::maxArguments() const { return 999; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ElementVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments (which are Element names) and add them to our list
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the Element?
        auto el = Elements::element(parser.argsv(n));
        if (el == Elements::Unknown)
            return Messenger::error("Unrecognised Element '{}' found in list.\n", parser.argsv(n));

        // If the Element is in the list already, complain
        if (std::find(data_.begin(), data_.end(), el) != data_.end())
            return Messenger::error("Element '{}' specified in list twice.\n", parser.argsv(n));

        // All OK - add it to our selection list
        data_.push_back(el);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ElementVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Don't write anything if there are no items in the list
    if (data_.size() == 0)
        return true;

    // Loop over the Element list
    std::string elements;
    for (auto el : data_)
        elements += fmt::format("  {}", Elements::symbol(el));

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, elements))
        return false;

    return true;
}
