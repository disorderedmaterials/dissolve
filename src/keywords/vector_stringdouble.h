// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <tuple>
#include <vector>

using StringDoubleVectorKeywordData = std::vector<std::pair<std::string, double>>;

// Keyword with list of pairs of std::string and double
class StringDoubleVectorKeyword : public KeywordBase
{
    public:
    explicit StringDoubleVectorKeyword(StringDoubleVectorKeywordData &data);
    ~StringDoubleVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    StringDoubleVectorKeywordData &data_;

    public:
    // Return reference to data
    StringDoubleVectorKeywordData &data();
    const StringDoubleVectorKeywordData &data() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
