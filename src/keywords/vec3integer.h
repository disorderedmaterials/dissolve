// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"
#include "vec3labels.h"

// Forward Declarations
/* none */

// Keyword with Integer Triplet Data
class Vec3IntegerKeyword : public KeywordData<Vec3<int>>
{
    public:
    Vec3IntegerKeyword(Vec3<int> value, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    Vec3IntegerKeyword(Vec3<int> value, Vec3<int> minValue, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    Vec3IntegerKeyword(Vec3<int> value, Vec3<int> minValue, Vec3<int> maxValue,
                       Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3IntegerKeyword();

    /*
     * Data Validation
     */
    private:
    // Validation limits to apply (if any)
    Vec3<bool> minimumLimit_, maximumLimit_;
    // Validation range (if appropriate)
    Vec3<int> min_, max_;

    public:
    // Return whether a minimum validation limit has been set for supplied index
    bool hasValidationMin(int index);
    // Return validation minimum limit for supplied index
    int validationMin(int index);
    // Return whether a maximum validation limit has been set for supplied index
    bool hasValidationMax(int index);
    // Return validation maximum limit for supplied index
    int validationMax(int index);
    // Validate supplied value
    bool isValid(Vec3<int> value);
    // Validate supplied single
    bool isValid(int index, int value);

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
     * Conversion
     */
    public:
    // Return value (as Vec3<int>)
    Vec3<int> asVec3Int();
    // Return value (as Vec3<double>)
    Vec3<double> asVec3Double();
};
