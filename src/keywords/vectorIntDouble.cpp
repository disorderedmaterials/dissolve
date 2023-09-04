// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/vectorIntDouble.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include <algorithm>

IntegerDoubleVectorKeyword::IntegerDoubleVectorKeyword::IntegerDoubleVectorKeyword(IntegerDoubleVectorKeywordData &data,
                                                                                   int nRequiredIntegers,
                                                                                   std::optional<int> nRequiredValues)
    : KeywordBase(typeid(this)), data_(data), nRequiredIntegers_(nRequiredIntegers), nRequiredValues_(nRequiredValues)
{
}

/*
 * Data
 */

// Return reference to data
IntegerDoubleVectorKeywordData &IntegerDoubleVectorKeyword::data() { return data_; }
const IntegerDoubleVectorKeywordData &IntegerDoubleVectorKeyword::data() const { return data_; }

// Return minimum number of arguments accepted
int IntegerDoubleVectorKeyword::minArguments() const { return (nRequiredIntegers_ + nRequiredValues_.value_or(1)); }

// Return maximum number of arguments accepted
std::optional<int> IntegerDoubleVectorKeyword::maxArguments() const
{
    if (nRequiredValues_)
        return nRequiredIntegers_ + nRequiredValues_.value();
    else
        return std::nullopt;
}

// Deserialise from supplied LineParser, starting at given argument offset
bool IntegerDoubleVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    std::vector<int> i;
    std::vector<double> d;

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
        d.push_back(parser.argd(n));
    if (d.size() < nRequiredValues_.value_or(1))
    {
        if (nRequiredValues_ == -1)
            return Messenger::error("Keyword {} expects at least one double value in addition.\n", name());
        else
            return Messenger::error("Keyword {} expects exactly {} double {} in addition.\n", name(), nRequiredValues_.value(),
                                    nRequiredValues_.value() == 1 ? "value" : "values");
    }

    // Add tuple to vector
    data_.emplace_back(i, d);

    return true;
}

// Serialise data to specified LineParser
bool IntegerDoubleVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &idData : data_)
    {
        std::string line;
        for (const auto &i : std::get<0>(idData))
            line += fmt::format("  {}", i - 1);
        for (const auto &d : std::get<1>(idData))
            line += fmt::format("  {}", d);

        if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, line))
            return false;
    }

    return true;
}

// Express as a serialisable value
SerialisedValue IntegerDoubleVectorKeyword::serialise() const
{
    return fromVector(data_,
                      [](auto pair) -> SerialisedValue {
                          return {{"indices", std::get<0>(pair)}, {"values", std::get<1>(pair)}};
                      });
}

// Read values from a serialisable value
void IntegerDoubleVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this](const auto &item) {
                 data_.emplace_back(toml::find<std::vector<int>>(item, "indices"),
                                    toml::find<std::vector<double>>(item, "values"));
             });
}

// Has not changed from initial value
bool IntegerDoubleVectorKeyword::isDefault() const { return data_.empty(); }
