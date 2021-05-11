// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with Double Data
class DoubleKeyword : public KeywordData<double>
{
    public:
    DoubleKeyword(double value);
    DoubleKeyword(double value, double minValue);
    DoubleKeyword(double value, double minValue, double maxValue);
    ~DoubleKeyword() override;

    /*
     * Data Validation
     */
    private:
    // Validation limits to apply (if any)
    bool minimumLimit_, maximumLimit_;
    // Validation range (if appropriate)
    double min_, max_;

    public:
    // Return whether a minimum validation limit has been set
    bool hasValidationMin();
    // Return validation minimum limit
    double validationMin();
    // Return whether a maximum validation limit has been set
    bool hasValidationMax();
    // Return validation maximum limit
    double validationMax();
    // Validate supplied value
    bool isValid(double value) override;

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

    /*
     * Conversion
     */
    public:
    // Return value (as bool)
    bool asBool() override;
    // Return value (as int)
    int asInt() override;
    // Return value (as double)
    double asDouble() override;
    // Return value (as string)
    std::string asString() override;
};
