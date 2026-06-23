// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "math/vector3.h"
#include <optional>

// Keyword managing Vector3
class Vec3DoubleKeyword : public KeywordBase
{
    public:
    explicit Vec3DoubleKeyword(Vector3 &data, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    explicit Vec3DoubleKeyword(Vector3 &data, std::optional<Vector3> minValue = std::nullopt,
                               std::optional<Vector3> maxValue = std::nullopt,
                               Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);

    ~Vec3DoubleKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    Vector3 &data_;
    // Initial Value
    const Vector3 default_;
    // Optional limits to apply
    std::optional<Vector3> minimumLimit_, maximumLimit_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Set data
    bool setData(Vector3 value);
    // Return data
    const Vector3 &data() const;
    // Return minimum limit
    std::optional<Vector3> minimumLimit() const;
    // Return maximum limit
    std::optional<Vector3> maximumLimit() const;

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
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
