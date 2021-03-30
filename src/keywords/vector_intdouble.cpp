// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/vector_intdouble.h"
#include "base/lineparser.h"
#include "classes/coredata.h"

IntegerDoubleVectorKeyword::IntegerDoubleVectorKeyword::IntegerDoubleVectorKeyword(IntegerDoubleVectorKeywordData &data,
                                                                                   int nRequiredIntegers,
                                                                                   std::optional<int> nRequiredValues)
    : KeywordData<IntegerDoubleVectorKeywordData &>(KeywordBase::VectorIntegerDoubleData, data),
      nRequiredIntegers_(nRequiredIntegers), nRequiredValues_(nRequiredValues)
{
}

IntegerDoubleVectorKeyword::~IntegerDoubleVectorKeyword() {}

// Return minimum number of arguments accepted
int IntegerDoubleVectorKeyword::minArguments() const { return (nRequiredIntegers_ + nRequiredValues_.value_or(1)); }

// Return maximum number of arguments accepted
int IntegerDoubleVectorKeyword::maxArguments() const { return (nRequiredIntegers_ + nRequiredValues_.value_or(99)); }

// Parse arguments from supplied LineParser, starting at given argument offset
bool IntegerDoubleVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
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

    hasBeenSet();

    return true;
}

// Write keyword data to specified LineParser
bool IntegerDoubleVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &idData : data_)
    {
        std::string line;
        for (const auto &i : std::get<0>(idData))
            line += fmt::format("  {}", i);
        for (const auto &d : std::get<1>(idData))
            line += fmt::format("  '{}'", d);

        if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, line))
            return false;
    }

    return true;
}
