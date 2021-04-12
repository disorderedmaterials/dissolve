// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtype.h"
#include "keywords/data.h"
#include "templates/reflist.h"

// Forward Declarations
class Configuration;

// Keyword with Element vector data
class ElementVectorKeyword : public KeywordData<std::vector<Elements::Element> &>
{
    public:
    ElementVectorKeyword(std::vector<Elements::Element> &targetData);
    ~ElementVectorKeyword();

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
