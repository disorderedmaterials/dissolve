// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <tuple>
#include <vector>

using StringDoubleVectorKeywordData = std::vector<std::pair<std::string, double>>;

// Keyword managing vector of pairs of std::string and double
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
    // Express as a tree node
    SerialisedValue serialise() const override;
    // Has not changed from inital value
    bool isDefault() const override;
    // Read values from a tree node
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
