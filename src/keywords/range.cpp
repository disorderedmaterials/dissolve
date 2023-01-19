// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/range.h"
#include "base/lineparser.h"

RangeKeyword::RangeKeyword(Range &data, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), default_(data), labelType_(labelType)
{
}

/*
 * Data
 */

// Return reference to data
Range &RangeKeyword::data() { return data_; }
const Range &RangeKeyword::data() const { return data_; }

// Label type to display in GUI
Vec3Labels::LabelType RangeKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int RangeKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> RangeKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool RangeKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg + 1))
    {
        data_.set(parser.argd(startArg), parser.argd(startArg + 1));

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool RangeKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.minimum(), data_.maximum());
}

// Express as a serialisable value
SerialisedValue RangeKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void RangeKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_.deserialise(node); }

// Has not changed from initial value
bool RangeKeyword::isDefault() const { return data_ == default_; }
