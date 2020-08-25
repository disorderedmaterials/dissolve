/*
    *** Number Format
    *** src/gui/render/numberformat.h
    Copyright T. Youngs 2013-2020

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

#include "base/enumoptions.h"
#include <QString>

// Forward Declarations
/* none */

// Number Format
class NumberFormat
{
    public:
    NumberFormat();
    ~NumberFormat();
    // Basic Number Type
    enum FormatType
    {
        DecimalFormat,
        IntegerFormat,
        ScientificFormat,
        nNumberFormats
    };
    // Return enum options for FormatType
    static EnumOptions<NumberFormat::FormatType> &formatTypes();

    /*
     * Definition
     */
    private:
    // Format type
    FormatType type_;
    // Number of decimals to use
    int nDecimals_;
    // Whether to force display of preceding '+' as well as '-'
    bool forcePrecedingPlus_;
    // Whether or not use use uppercase exponent character (for ScientificFormat)
    bool useUpperCaseExponent_;
    // Whether to use 'E' notation in preference to 'x10' notation
    bool useENotation_;

    public:
    // Set format type
    void setType(NumberFormat::FormatType type);
    // Return format type
    NumberFormat::FormatType type();
    // Set number of decimals to use
    void setNDecimals(int n);
    // Return number of decimals to use
    int nDecimals();
    // Set whether to force display of preceding '+' as well as '-'
    void setForcePrecedingPlus(bool b);
    // Return whether to force display of preceding '+' as well as '-'
    bool forcePrecedingPlus();
    // Set whether or not use use uppercase exponent character (for ScientificFormat)
    void setUseUpperCaseExponent(bool b);
    // Return whether or not use use uppercase exponent character (for ScientificFormat)
    bool useUpperCaseExponent();
    // Set whether to use 'E' notation in preference to 'x10' notation
    void setUseENotation(bool b);
    // Return whether to use 'E' notation in preference to 'x10' notation
    bool useENotation();

    /*
     * Number Conversion
     */
    public:
    // Return number formatted according to internal definition
    QString format(double number);
};
