// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "keywords/vec3labels.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class ProcedureNode;

// Keyword with NodeValue Triplet Data
class Vec3NodeValueKeyword : public KeywordData<Vec3<NodeValue>>
{
    public:
    Vec3NodeValueKeyword(ProcedureNode *parentNode, Vec3<double> value, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3NodeValueKeyword();

    /*
     * Parent Node
     */
    private:
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

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

    /*
     * Set
     */
    public:
    // Set the specified value from supplied expression text
    bool setValue(int index, std::string_view expressionText);

    /*
     * Conversion
     */
    public:
    // Return value (as Vec3<int>)
    Vec3<int> asVec3Int();
    // Return value (as Vec3<double>)
    Vec3<double> asVec3Double();
};
