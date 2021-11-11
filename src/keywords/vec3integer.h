// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "vec3labels.h"

// Keyword with Vec3<int>
class Vec3IntegerKeyword : public KeywordBase
{
    public:
    explicit Vec3IntegerKeyword(Vec3<int> &data, std::optional<Vec3<int>> minValue = std::nullopt,
                                std::optional<Vec3<int>> maxValue = std::nullopt,
                                Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3IntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    Vec3<int> &data_;
    // Validation limits to apply (if any)
    std::optional<Vec3<int>> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool setData(Vec3<int> value);
    // Return data
    const Vec3<int> &data() const;
    // Return validation minimum limit
    std::optional<Vec3<int>> validationMin();
    // Return validation maximum limit
    std::optional<Vec3<int>> validationMax();

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
