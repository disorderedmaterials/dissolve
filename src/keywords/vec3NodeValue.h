// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/vec3Labels.h"
#include "procedure/nodeValue.h"
#include "templates/vector3.h"

// Forward Declarations
class ProcedureNode;

// Keyword managing Vec3<NodeValue>
class Vec3NodeValueKeyword : public KeywordBase
{
    public:
    Vec3NodeValueKeyword(Vec3<NodeValue> &data, ProcedureNode *parentNode,
                         Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3NodeValueKeyword() override = default;

    /*
     * Data
     */
    private:
    // Initial value
    Vec3<NodeValue> default_;
    // Reference to data
    Vec3<NodeValue> &data_;
    // Parent ProcedureNode
    ProcedureNode *parentNode_;
    // Label type to display in GUI
    Vec3Labels::LabelType labelType_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Return reference to data
    const Vec3<NodeValue> &data() const;
    // Set data
    bool setData(const Vec3<NodeValue> &v);
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
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
