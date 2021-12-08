// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Keyword managing std::string data
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
    dissolve::any_ptr data() const override;

    /*
     * Arguments
     */
    public:
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
