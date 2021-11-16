// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>
#include <tuple>
#include <vector>

using IntegerDoubleVectorKeywordData = std::vector<std::tuple<std::vector<int>, std::vector<double>>>;

// Keyword with list of Tuples of Vectors
class IntegerDoubleVectorKeyword : public KeywordBase
{
    public:
    explicit IntegerDoubleVectorKeyword(IntegerDoubleVectorKeywordData &data, int nRequiredIntegers,
                                        std::optional<int> nRequiredValues = std::nullopt);
    ~IntegerDoubleVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    IntegerDoubleVectorKeywordData &data_;
    // Number of required integers
    int nRequiredIntegers_;
    // Number of required values (or, if not specified, one or more)
    std::optional<int> nRequiredValues_;

    public:
    // Return reference to data
    IntegerDoubleVectorKeywordData &data();
    const IntegerDoubleVectorKeywordData &data() const;

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
