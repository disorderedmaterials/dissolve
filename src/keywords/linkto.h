// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with Link to Other Keyword
class LinkToKeyword : public KeywordData<KeywordBase *>
{
    public:
    LinkToKeyword(KeywordBase *keywordData);
    ~LinkToKeyword();

    /*
     * Base Pointer Return (Overloading KeywordBase virtual)
     */
    public:
    // Return base pointer for this (may be overloaded to provide access to other KeywordBase instance)
    KeywordBase *base();

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
