// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with Integer Data
class IntegerKeyword : public KeywordData<int>
{
    public:
    IntegerKeyword(int value);
    IntegerKeyword(int value, int minValue);
    IntegerKeyword(int value, int minValue, int maxValue);
    ~IntegerKeyword();

    /*
     * Data Validation
     */
    private:
    // Validation limits to apply (if any)
    bool minimumLimit_, maximumLimit_;
    // Validation range (if appropriate)
    int min_, max_;

    public:
    // Return whether a minimum validation limit has been set
    bool hasValidationMin();
    // Return validation minimum limit
    int validationMin();
    // Return whether a maximum validation limit has been set
    bool hasValidationMax();
    // Return validation maximum limit
    int validationMax();
    // Validate supplied value
    bool isValid(int value);

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
    bool asBool();
    // Return value (as int)
    int asInt();
    // Return value (as double)
    double asDouble();
    // Return value (as string)
    std::string asString();
};
