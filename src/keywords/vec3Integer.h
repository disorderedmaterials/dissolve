// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "math/vector3i.h"
#include <optional>

// Keyword managing Vector3i
class Vec3IntegerKeyword : public KeywordBase
{
    public:
    explicit Vec3IntegerKeyword(Vector3i &data, std::optional<Vector3i> minValue = std::nullopt,
                                std::optional<Vector3i> maxValue = std::nullopt,
                                Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3IntegerKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to target data
    Vector3i &data_;
    // Initial Value
    const Vector3i default_;
    // Optional limits to apply
    std::optional<Vector3i> minimumLimit_, maximumLimit_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Set data
    bool setData(Vector3i value);
    // Return data
    const Vector3i &data() const;
    // Return minimum limit
    std::optional<Vector3i> minimumLimit() const;
    // Return maximum limit
    std::optional<Vector3i> maximumLimit() const;

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
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
