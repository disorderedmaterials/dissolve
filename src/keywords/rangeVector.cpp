// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/rangeVector.h"
#include "base/lineParser.h"
#include "classes/coreData.h"

RangeVectorKeyword::RangeVectorKeyword(std::vector<Range> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return the data vector
std::vector<Range> &RangeVectorKeyword::data() { return data_; }
const std::vector<Range> &RangeVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool RangeVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments
    for (auto n = startArg; n < parser.nArgs(); n += 2)
    {
        // Check to see if value pair are numbers
        if (DissolveSys::isNumber(parser.argsv(n)) && DissolveSys::isNumber(parser.argsv(n + 1)))
        {
            data_.emplace_back(parser.argd(n), parser.argd(n + 1));
        }
        else
        {
            return Messenger::error("Value '{}' given to keyword '{}' doesn't appear to be numerical.\n",
                                    DissolveSys::isNumber(parser.argsv(n)) ? parser.argsv(n + 1) : parser.argsv(n), name());
        }
    }

    return true;
}

// Serialise data to specified LineParser
bool RangeVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto &range : data_)
    {
        // Serialising, skipping undefined ranges
        if (range.isDefined() && !parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, range.minimum(), range.maximum()))
        {
            return false;
        }
    }

    return true;
}

/*
 * Object Management
 */

// Express as a serialisable value
SerialisedValue RangeVectorKeyword::serialise() const
{
    return fromVector(data_, [](const auto &item) -> SerialisedValue { return item.serialise(); });
}

// Read values from a serialisable value
void RangeVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node, [this](const auto &item)
             { data_.emplace_back(toml::find<double>(item, "min"), toml::find<double>(item, "max")); });
}

// Has not changed from initial value
bool RangeVectorKeyword::isDefault() const { return data_.empty(); }