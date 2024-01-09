// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    // Initial value
    const int default_;
    // Optional limits to apply
    std::optional<int> minimumLimit_, maximumLimit_;
    // Whether the data has been set
    bool set_{false};

    public:
    // Set data
    bool setData(int value);
    // Return data
    int data() const;
    // Return minimum limit
    std::optional<int> minimumLimit() const;
    // Return maximum limit
    std::optional<int> maximumLimit() const;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};
