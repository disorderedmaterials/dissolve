// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include <optional>
#include <tuple>
#include <vector>

using StringPairVectorKeywordData = std::vector<std::pair<std::string, std::string>>;

// Keyword with list of pairs of std::string
class StringPairVectorKeyword : public KeywordData<StringPairVectorKeywordData &>
{
    public:
    StringPairVectorKeyword(StringPairVectorKeywordData &data);
    ~StringPairVectorKeyword() override;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
