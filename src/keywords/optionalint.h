// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>

// Keyword managing optional int data
class OptionalIntegerKeyword : public KeywordBase
{
    public:
    explicit OptionalIntegerKeyword(std::optional<int> &data, int minValue, std::optional<int> maxValue, int valueDelta,
                                    std::string_view textWhenNull);
    ~OptionalIntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    std::optional<int> &data_;
    // Minimum allowed value, below which equates to nullopt
    int minimumLimit_;
    // Maximum limit to apply (if any)
    std::optional<int> maximumLimit_;
    // Step size for widget
    int valueDelta_;
    // Text to display in widget when null
    std::string textWhenNull_;
    // Whether the data has been set
    bool set_{false};

    public:
    // Set data
    bool setData(std::optional<int> value);
    // Return data
    std::optional<int> data() const;
    // Return validation minimum limit
    int validationMin() const;
    // Return validation maximum limit
    std::optional<int> validationMax() const;
    // Return step size for widget
    int valueDelta() const;
    // Return text to display in widget  when value is null
    std::string textWhenNull() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
