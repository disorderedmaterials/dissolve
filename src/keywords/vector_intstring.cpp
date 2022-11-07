// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/vector_intstring.h"
#include "base/lineparser.h"
#include "classes/coredata.h"

IntegerStringVectorKeyword::IntegerStringVectorKeyword(IntegerStringVectorKeywordData &data, int nRequiredIntegers,
                                                       std::optional<int> nRequiredValues)
    : KeywordBase(typeid(this)), data_(data), nRequiredIntegers_(nRequiredIntegers), nRequiredValues_(nRequiredValues)
{
}

/*
 * Data
 */

// Return reference to data
IntegerStringVectorKeywordData &IntegerStringVectorKeyword::data() { return data_; }
const IntegerStringVectorKeywordData &IntegerStringVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IntegerStringVectorKeyword::minArguments() const { return (nRequiredIntegers_ + nRequiredValues_.value_or(1)); }

// Return maximum number of arguments accepted
std::optional<int> IntegerStringVectorKeyword::maxArguments() const
{
    if (nRequiredValues_)
        return nRequiredIntegers_ + nRequiredValues_.value();
    else
        return std::nullopt;
}

// Deserialise from supplied LineParser, starting at given argument offset
bool IntegerStringVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    std::vector<int> i;
    std::vector<std::string> s;

    auto argIndex = startArg;

    // Read integer values
    for (auto n = 0; n < nRequiredIntegers_; ++n, ++argIndex)
    {
        if (parser.hasArg(argIndex))
            i.push_back(parser.argi(argIndex));
        else
            return false;
    }

    // Read values and check number
    for (auto n = argIndex; n < parser.nArgs(); ++n)
        s.emplace_back(parser.argsv(n));
    if (s.size() < nRequiredValues_.value_or(1))
    {
        if (nRequiredValues_ == -1)
            return Messenger::error("Keyword {} expects at least one double value in addition.\n", name());
        else
            return Messenger::error("Keyword {} expects exactly {} double {} in addition.\n", name(), nRequiredValues_.value(),
                                    nRequiredValues_.value() == 1 ? "value" : "values");
    }

    // Add tuple to vector
    data_.emplace_back(i, s);

    return true;
}

// Serialise data to specified LineParser
bool IntegerStringVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &d : data_)
    {
        std::string line;
        for (const auto &i : std::get<0>(d))
            line += fmt::format("  {}", i);
        for (const auto &s : std::get<1>(d))
            line += fmt::format("  '{}'", s);

        if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, line))
            return false;
    }

    return true;
}

// Express as a tree node
SerialisedValue IntegerStringVectorKeyword::serialise() const
{
    std::vector<SerialisedValue> result;
    std::transform(data_.begin(), data_.end(), std::back_inserter(result), [](auto pair) -> SerialisedValue {
        return {{"indices", std::get<0>(pair)}, {"values", std::get<1>(pair)}};
    });
    return result;
}

// Read values from a tree node
void IntegerStringVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto item : node.as_array())
        data_.emplace_back(toml::find<std::vector<int>>(item, "indices"), toml::find<std::vector<std::string>>(item, "values"));
}

// Has not changed from initial value
bool IntegerStringVectorKeyword::isDefault() const { return data_.empty(); }
