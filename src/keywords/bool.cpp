// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/bool.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

BoolKeyword::BoolKeyword(bool &data) : KeywordBase(typeid(this)), data_(data), default_(data) {}

/*
 * Data
 */

// Has not changed from initial value
bool BoolKeyword::isDefault() const { return data_ == default_; }

// Set data
bool BoolKeyword::setData(bool value)
{
    data_ = value;
    set_ = true;

    return true;
}

// Return data
const bool &BoolKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool BoolKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (parser.hasArg(startArg))
    {
        data_ = parser.argb(startArg);
        set_ = true;

        return true;
    }
    return false;
}

// Serialise data to specified LineParser
bool BoolKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!set_)
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName, DissolveSys::btoa(data_));
}

// Express as a serialisable value
SerialisedValue BoolKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void BoolKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData) { data_ = node.as_boolean(); }
