// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/elementvector.h"
#include "base/lineparser.h"
#include "data/elements.h"
#include "templates/algorithms.h"

ElementVectorKeyword::ElementVectorKeyword(std::vector<Elements::Element> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return if the current data object is empty
bool ElementVectorKeyword::isDataEmpty() const { return data_.empty(); }

// Return reference to data
std::vector<Elements::Element> &ElementVectorKeyword::data() const { return data_; }
const std::vector<Elements::Element> &ElementVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> ElementVectorKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool ElementVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments which are Element names
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the Element?
        auto el = Elements::element(parser.argsv(n));
        if (el == Elements::Unknown)
            return Messenger::error("Unrecognised Element '{}' given to keyword '{}'.\n", parser.argsv(n), name());

        // If the Element is already present, complain
        if (std::find(data_.begin(), data_.end(), el) != data_.end())
            return Messenger::error("Element '{}' specified twice in keyword '{}'.\n", parser.argsv(n), name());

        // All OK - add it to our vector
        data_.push_back(el);
    }

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool ElementVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    return parser.writeLineF("{}{}{}\n", prefix, keywordName,
                             joinStrings(data_, "  ", [](const auto &el) { return Elements::symbol(el); }));
}
