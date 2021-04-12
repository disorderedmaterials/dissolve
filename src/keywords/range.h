// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "math/range.h"
#include "vec3labels.h"

// Forward Declarations
/* none */

// Keyword with Double Triplet Data
class RangeKeyword : public KeywordData<Range>
{
    public:
    RangeKeyword(Range value, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~RangeKeyword();

    /*
     * Label Type
     */
    private:
    // Label type to display in GUI
    Vec3Labels::LabelType labelType_;

    public:
    // Label type to display in GUI
    Vec3Labels::LabelType labelType() const;

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
