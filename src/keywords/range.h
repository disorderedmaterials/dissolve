// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "math/range.h"
#include "vec3labels.h"

// Keyword managing Range data
class RangeKeyword : public KeywordBase
{
    public:
    RangeKeyword(Range &data, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~RangeKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Range &data_;
    // Initial Value
    const Range default_;
    // Label type to display in GUI
    Vec3Labels::LabelType labelType_;

    public:
    // Return reference to data
    Range &data();
    const Range &data() const;
    // Label type to display in GUI
    Vec3Labels::LabelType labelType() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a tree node
    SerialisedValue serialise() const override;
    // Has not changed from inital value
    bool isDefault() const override;
    // Read values from a tree node
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
