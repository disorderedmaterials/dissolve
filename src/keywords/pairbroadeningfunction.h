// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "math/pairbroadeningfunction.h"

// Forward Declarations
/* none */

// Keyword with PairBroadeningFunction Data
class PairBroadeningFunctionKeyword : public KeywordData<PairBroadeningFunction>
{
    public:
    PairBroadeningFunctionKeyword(PairBroadeningFunction value);
    ~PairBroadeningFunctionKeyword();

    /*
     * Data Validation
     */
    public:
    // Validate supplied value
    bool isValid(PairBroadeningFunction value);

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
