/*
    *** Keyword - Double
    *** src/keywords/double.h
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

// Forward Declarations
/* none */

// Keyword with Double Data
class DoubleKeyword : public KeywordData<double>
{
    public:
    DoubleKeyword(double value);
    DoubleKeyword(double value, double minValue);
    DoubleKeyword(double value, double minValue, double maxValue);
    ~DoubleKeyword();

    /*
     * Data Validation
     */
    private:
    // Validation limits to apply (if any)
    bool minimumLimit_, maximumLimit_;
    // Validation range (if appropriate)
    double min_, max_;

    public:
    // Return whether a minimum validation limit has been set
    bool hasValidationMin();
    // Return validation minimum limit
    double validationMin();
    // Return whether a maximum validation limit has been set
    bool hasValidationMax();
    // Return validation maximum limit
    double validationMax();
    // Validate supplied value
    bool isValid(double value);

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
    // Return value (as bool)
    bool asBool();
    // Return value (as int)
    int asInt();
    // Return value (as double)
    double asDouble();
    // Return value (as string)
    std::string asString();
};
