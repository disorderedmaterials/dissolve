// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Keyword with Double Data
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
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
