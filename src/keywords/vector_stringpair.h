// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>
#include <tuple>
#include <vector>

using StringPairVectorKeywordData = std::vector<std::pair<std::string, std::string>>;

// Keyword managing vector of pairs of std::string
class StringPairVectorKeyword : public KeywordBase
{
    public:
    explicit StringPairVectorKeyword(StringPairVectorKeywordData &data);
    ~StringPairVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    StringPairVectorKeywordData &data_;

    public:
    // Return reference to data
    StringPairVectorKeywordData &data();
    std::any_ptr data() const override;

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
