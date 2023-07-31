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
        if (DissolveSys::isNumber(parser.argsv(n)))
        {
            if (DissolveSys::isNumber(parser.argsv(n + 1)))
            {
                // If both are numbers, initialise new Range
                data_.emplace_back(parser.argd(n), parser.argd(n + 1));
            }
            else
            {
                return Messenger::error("Value '{}' provided for keyword '{}' doesn't appear to be a number.\n",
                                        parser.argsv(n + 1), name());
            }
        }
        else
        {
            return Messenger::error("Value '{}' provided for keyword '{}' doesn't appear to be a number.\n", parser.argsv(n),
                                    name());
        }
    }

    return true;
}

// Serialise data to specified LineParser
bool RangeVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto &range : data_)
        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, range.minimum(), range.maximum()))
            return false;

    return true;
}

/*
 * Object Management
 */

// Express as a serialisable value
SerialisedValue RangeVectorKeyword::serialise() const
{
    return fromVector(data_,
                      [](const auto &item) -> SerialisedValue {
                          return {{"minimum", item.minimum()}, {"maximum", item.maximum()}};
                      });
}

// Read values from a serialisable value
void RangeVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node, [this](const auto &item)
             { data_.emplace_back(toml::find<double>(item, "minimum"), toml::find<double>(item, "maximum")); });
}

// Has not changed from initial value
bool RangeVectorKeyword::isDefault() const { return data_.empty(); }