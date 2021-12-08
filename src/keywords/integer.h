// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>

// Keyword managing Integer data
class IntegerKeyword : public KeywordBase
{
    public:
    explicit IntegerKeyword(int &data, std::optional<int> minValue = std::nullopt, std::optional<int> maxValue = std::nullopt);
    ~IntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    int &data_;
    // Validation limits to apply (if any)
    std::optional<int> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool setData(int value);
    // Return data
    int data() const;
    // Return validation minimum limit
    std::optional<int> validationMin();
    // Return validation maximum limit
    std::optional<int> validationMax();
    // Display in GUI
    std::string toString() const override;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
