// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "templates/vector3.h"
#include <optional>

// Keyword managing Vec3<int>
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
    // Initial Value
    const Vec3<int> default_;
    // Optional limits to apply
    std::optional<Vec3<int>> minimumLimit_, maximumLimit_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Set data
    bool setData(Vec3<int> value);
    // Return data
    const Vec3<int> &data() const;
    // Return minimum limit
    std::optional<Vec3<int>> minimumLimit() const;
    // Return maximum limit
    std::optional<Vec3<int>> maximumLimit() const;

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
