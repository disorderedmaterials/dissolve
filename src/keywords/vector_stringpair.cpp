// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/vector_stringpair.h"
#include "base/lineparser.h"

StringPairVectorKeyword::StringPairVectorKeyword(StringPairVectorKeywordData &data)
    : KeywordData<StringPairVectorKeywordData &>(KeywordBase::VectorStringPairData, data)
{
}

StringPairVectorKeyword::~StringPairVectorKeyword() {}

// Return minimum number of arguments accepted
int StringPairVectorKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int StringPairVectorKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool StringPairVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Read value pairs
    for (auto n = startArg; n < parser.nArgs(); n += 2)
        data_.emplace_back(parser.argsv(n), parser.argsv(n + 1));

    hasBeenSet();

    return true;
}

// Write keyword data to specified LineParser
bool StringPairVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &[s1, s2] : data_)
        if (!parser.writeLineF("{}{}  '{}'  '{}'\n", prefix, keywordName, s1, s2))
            return false;

    return true;
}
