// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/rFactorRangesVector.h"
#include "base/lineParser.h"
#include "classes/coreData.h"

RFactorRangesVector::RFactorRangesVector(std::vector<Range> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return the data vector
std::vector<Range> &RFactorRangesVector::data() { return data_; }
const std::vector<Range> &RFactorRangesVector::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool RFactorRangesVector::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
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
                return Messenger::error("Value '{}' provided for keyword R Factor Ranges doesn't appear to be a number.\n",
                                        parser.argsv(n + 1));
            }
        }
        else
        {
            return Messenger::error("Value '{}' provided for keyword R Factor Ranges doesn't appear to be a number.\n",
                                    parser.argsv(n));
        }
    }

    return true;
}

// Serialise data to specified LineParser
bool RFactorRangesVector::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto range : data_)
        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, range.minimum(), range.maximum()))
            return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied range in the contained data
void RFactorRangesVector::removeReferencesTo(Range range)
{
    auto it = std::find_if(data_.begin(), data_.end(),
                           [&range](const Range &rg)
                           { return (range.minimum() == rg.minimum() && range.maximum() == rg.maximum()); });
    if (it != data_.end())
        data_.erase(it);
}

// Express as a serialisable value
SerialisedValue RFactorRangesVector::serialise() const
{
    return fromVector(data_,
                      [](const auto item) -> SerialisedValue {
                          return {{"minimum", item.minimum()}, {"maximum", item.maximum()}};
                      });
}

// Read values from a serialisable value
void RFactorRangesVector::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    Range range;
    auto counter = 0;

    for (auto &item : node.as_array())
    {
        if (counter % 2 != 0)
        {
            range.setMinimum(toml::get<double>(item));
        }
        else
        {
            range.setMaximum(toml::get<double>(item));
            data_.push_back(range);
        }

        ++counter;
    }
}

// Has not changed from initial value
bool RFactorRangesVector::isDefault() const { return data_.empty(); }