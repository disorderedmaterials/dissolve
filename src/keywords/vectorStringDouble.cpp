// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/vectorStringDouble.h"
#include "base/lineParser.h"

StringDoubleVectorKeyword::StringDoubleVectorKeyword(StringDoubleVectorKeywordData &data)
    : KeywordBase(typeid(this)), data_(data)
{
}

/*
 * Data
 */

// Return reference to data
StringDoubleVectorKeywordData &StringDoubleVectorKeyword::data() { return data_; }
const StringDoubleVectorKeywordData &StringDoubleVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */
// Return minimum number of arguments accepted
int StringDoubleVectorKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> StringDoubleVectorKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool StringDoubleVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Read value pairs
    for (auto n = startArg; n < parser.nArgs(); n += 2)
        data_.emplace_back(parser.argsv(n), parser.argd(n + 1));

    return true;
}

// Serialise data to specified LineParser
bool StringDoubleVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &[s, d] : data_)
        if (!parser.writeLineF("{}{}  '{}'  '{}'\n", prefix, keywordName, s, d))
            return false;

    return true;
}

// Express as a serialisable value
SerialisedValue StringDoubleVectorKeyword::serialise() const
{
    return fromVector(data_,
                      [](const auto &item) -> SerialisedValue {
                          return {{"name", item.first}, {"value", item.second}};
                      });
}

// Read values from a serialisable value
void StringDoubleVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node, [this](const auto &item)
             { data_.emplace_back(toml::find<std::string>(item, "name"), toml::find<double>(item, "value")); });
}

// Has not changed from initial value
bool StringDoubleVectorKeyword::isDefault() const { return data_.empty(); }