// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "generator/nodeValue.h"
#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "math/vector3.h"

// Forward Declarations
class GeneratorNode;

// Keyword managing Vector3NodeValue
class Vec3NodeValueKeyword : public KeywordBase
{
    public:
    Vec3NodeValueKeyword(Vector3NodeValue &data, GeneratorNode *parentNode,
                         Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3NodeValueKeyword() override = default;

    /*
     * Data
     */
    private:
    // Initial value
    Vector3NodeValue default_;
    // Reference to data
    Vector3NodeValue &data_;
    // Parent GeneratorNode
    GeneratorNode *parentNode_;
    // Label type to display in GUI
    Vec3Labels::LabelType labelType_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Return reference to data
    const Vector3NodeValue &data() const;
    // Set data
    bool setData(const Vector3NodeValue &v);
    // Set the specified value from supplied expression text
    bool setData(int index, std::string_view expressionText);
    // Return label type to display in GUI
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
    void serialize(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
