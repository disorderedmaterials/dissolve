// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include <optional>

// Keyword managing Double data
class DoubleKeyword : public KeywordBase
{
    public:
    explicit DoubleKeyword(double &data, std::optional<double> minValue = std::nullopt,
                           std::optional<double> maxValue = std::nullopt);
    ~DoubleKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    double &data_;
    // Validation limits to apply (if any)
    std::optional<double> minimumLimit_, maximumLimit_;
    // Whether the data has been set
    bool set_{false};

    public:
    // Set data
    bool setData(double value);
    // Return data
    double data() const;
    // Return validation minimum limit
    std::optional<double> validationMin();
    // Return validation maximum limit
    std::optional<double> validationMax();

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
