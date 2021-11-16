// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "keywords/base.h"

// Keyword with Element vector data
class ElementVectorKeyword : public KeywordBase
{
    public:
    explicit ElementVectorKeyword(std::vector<Elements::Element> &data);
    ~ElementVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    std::vector<Elements::Element> &data_;

    protected:
    // Return if the current data object is empty
    bool isDataEmpty() const override;

    public:
    // Return reference to data
    std::vector<Elements::Element> &data();
    const std::vector<Elements::Element> &data() const;

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
