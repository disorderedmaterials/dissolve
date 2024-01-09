// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "math/range.h"

// Keyword managing Range data
class RangeKeyword : public KeywordBase
{
    public:
    RangeKeyword(Range &data, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    RangeKeyword(Range &data, std::optional<double> minimumLimit, std::optional<double> maximumLimit,
                 Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~RangeKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    Range &data_;
    // Optional limits to apply
    std::optional<double> minimumLimit_, maximumLimit_;
    // Initial Value
    const Range default_;
    // Label type to display in GUI
    Vec3Labels::LabelType labelType_;

    public:
    // Set data
    bool setData(double rangeMin, double rangeMax);
    bool setData(const Range &range);
    // Set range minimum
    bool setMinimum(double rangeMin);
    // Set range maximum
    bool setMaximum(double rangeMax);
    // Return reference to data
    const Range &data() const;
    // Return minimum limit
    std::optional<double> minimumLimit() const;
    // Return maximum limit
    std::optional<double> maximumLimit() const;
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
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Has not changed from initial value
    bool isDefault() const override;
};
