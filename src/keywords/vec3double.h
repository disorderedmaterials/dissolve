// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3labels.h"
#include "templates/vector3.h"
#include <optional>

// Keyword managing Vec3<double>
class Vec3DoubleKeyword : public KeywordBase
{
    public:
    explicit Vec3DoubleKeyword(Vec3<double> &data, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    explicit Vec3DoubleKeyword(Vec3<double> &data, std::optional<Vec3<double>> minValue = std::nullopt,
                               std::optional<Vec3<double>> maxValue = std::nullopt,
                               Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);

    ~Vec3DoubleKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    Vec3<double> &data_;
    // Validation limits to apply (if any)
    std::optional<Vec3<double>> minimumLimit_, maximumLimit_;

    public:
    // Set data
    bool setData(Vec3<double> value);
    // Return data
    dissolve::any_ptr data() const override;
    // Return validation minimum limit
    std::optional<Vec3<double>> validationMin();
    // Return validation maximum limit
    std::optional<Vec3<double>> validationMax();

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
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
