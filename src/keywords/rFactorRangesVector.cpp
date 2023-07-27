// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "classes/coreData.h"
#include "keywords/rFactorRanges.h"
#include "math/range.h"

RFactorRangesVector::RFactorRangesVector(std::vector<Range *> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return the data vector
std::vector<Range *> &RFactorRangesVector::data() { return data_; }
const std::vector<Range *> &RFactorRangesVector::data() const { return data_; }

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
                data_.push_back(Range(parser.argd(n), parser.argd(n + 1)));
            }
            else
                return Messenger::error("Value '{}' provided for keyword R Factor Ranges doesn't appear to be a number.\n",
                                        parser.argsv(n + 1));
        }
        else
            return Messenger::error("Value '{}' provided for keyword R Factor Ranges doesn't appear to be a number.\n",
                                    parser.argsv(n));
    }

    return true;
}

// Serialise data to specified LineParser
bool RFactorRangesVector::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto *range : data_)
        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, range->minimum().asString(), range->maximum().asString()))
            return false;
}

/*
 * Object Management
 */

// Prune any references to the supplied range in the contained data
void RFactorRangesVector::removeReferencesTo(Range *range)
{
    auto it = std::find(data_.begin(), data_.end(), range);
    if (it != data_.end())
        data_.erase(it);
}

// Express as a serialisable value
SerialisedValue RFactorRangesVector::serialise() const
{
    return fromVector(data_, [](const auto *item) { return item->name(); });
}

// Read values from a serialisable value
void RFactorRangesVector::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this, &coreData](const auto &item)
             {
                 // Set the value
                 bool isFloatingPoint = false;
                 if (DissolveSys::isNumber(parser.argsv(n), isFloatingPoint))
                 {
                     // All OK - add it to our vector
                     if (isFloatingPoint)
                         data_.push_back(parser.argd(n));
                     else
                         data_.push_back(parser.argi(n));
                 }
                 else
                     return Messenger::error("Value '{}' provided for keyword R Factor Ranges doesn't appear to be a number.\n",
                                             parser.argsv(n));
             });
}

// Has not changed from initial value
bool RFactorRangesVector::isDefault() const { return data_.empty(); }