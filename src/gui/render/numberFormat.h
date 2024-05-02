// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include <QString>

// Number Format
class NumberFormat
{
    public:
    // Basic Number Type
    enum class FormatType
    {
        Decimal,
        Integer,
        Scientific
    };
    // Return enum options for FormatType
    static EnumOptions<NumberFormat::FormatType> formatTypes();
    NumberFormat(FormatType type = FormatType::Decimal, int nDecimals = 1);
    ~NumberFormat() = default;

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
