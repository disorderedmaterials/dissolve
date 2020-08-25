/*
    *** Keyword - Double Triplet
    *** src/keywords/vec3double.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "keywords/data.h"
#include "vec3labels.h"

// Forward Declarations
/* none */

// Keyword with Double Triplet Data
class Vec3DoubleKeyword : public KeywordData<Vec3<double>>
{
    public:
    Vec3DoubleKeyword(Vec3<double> value, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    Vec3DoubleKeyword(Vec3<double> value, Vec3<double> minValue, Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    Vec3DoubleKeyword(Vec3<double> value, Vec3<double> minValue, Vec3<double> maxValue,
                      Vec3Labels::LabelType labelType = Vec3Labels::NoLabels);
    ~Vec3DoubleKeyword();

    /*
     * Data Validation
     */
    private:
    // Validation limits to apply (if any)
    Vec3<bool> minimumLimit_, maximumLimit_;
    // Validation range (if appropriate)
    Vec3<double> min_, max_;

    public:
    // Return whether a minimum validation limit has been set for supplied index
    bool hasValidationMin(int index);
    // Return validation minimum limit for supplied index
    double validationMin(int index);
    // Return whether a maximum validation limit has been set for supplied index
    bool hasValidationMax(int index);
    // Return validation maximum limit for supplied index
    double validationMax(int index);
    // Validate supplied value
    bool isValid(Vec3<double> value);
    // Validate supplied single
    bool isValid(int index, double value);

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
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);

    /*
     * Conversion
     */
    public:
    // Return value (as Vec3<int>)
    Vec3<int> asVec3Int();
    // Return value (as Vec3<double>)
    Vec3<double> asVec3Double();
};
