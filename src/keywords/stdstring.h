// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Keyword with std::string Data
class StringKeyword : public KeywordBase
{
    public:
    explicit StringKeyword(std::string &data);
    ~StringKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::string &data_;

    public:
    // Return reference to data
    std::string &data();
    const std::string &data() const;

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
