// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/numberFormat.h"
#include "math/doubleExp.h"
#include <QString>

NumberFormat::NumberFormat(FormatType type, int nDecimals)
    : type_(type), nDecimals_(nDecimals), forcePrecedingPlus_(false), useUpperCaseExponent_(true), useENotation_(true)
{
}

// Return enum options for FormatType
EnumOptions<NumberFormat::FormatType> NumberFormat::formatTypes()
{
    return EnumOptions<NumberFormat::FormatType>("FormatType", {{NumberFormat::FormatType::Decimal, "Decimal"},
                                                                {NumberFormat::FormatType::Integer, "Integer"},
                                                                {NumberFormat::FormatType::Scientific, "Scientific"}});
}

/*
 * Definition
 */

// Set format type
void NumberFormat::setType(NumberFormat::FormatType type) { type_ = type; }

// Return format type
NumberFormat::FormatType NumberFormat::type() { return type_; }

// Set number of decimals to use
void NumberFormat::setNDecimals(int n) { nDecimals_ = n; }

// Return number of decimals to use
int NumberFormat::nDecimals() { return nDecimals_; }

// Set whether to force display of preceding '+' as well as '-'
void NumberFormat::setForcePrecedingPlus(bool b) { forcePrecedingPlus_ = b; }

// Return whether to force display of preceding '+' as well as '-'
bool NumberFormat::forcePrecedingPlus() { return forcePrecedingPlus_; }

// Set whether or not use use uppercase exponent character (for ScientificFormat)
void NumberFormat::setUseUpperCaseExponent(bool b) { useUpperCaseExponent_ = b; }

// Return whether or not use use uppercase exponent character (for ScientificFormat)
bool NumberFormat::useUpperCaseExponent() { return useUpperCaseExponent_; }

// Set whether to use 'E' notation in preference to 'x10' notation
void NumberFormat::setUseENotation(bool b) { useENotation_ = b; }

// Return whether to use 'E' notation in preference to 'x10' notation
bool NumberFormat::useENotation() { return useENotation_; }

/*
 * Number Conversion
 */

// Return number formatted according to internal definition
QString NumberFormat::format(double number)
{
    QString result;

    DoubleExp x(number);

    // Add '+' onto string if requested and number is positive
    if (forcePrecedingPlus_ && (number >= 0.0))
        result = "+";

    // Construct rest of string
    switch (type_)
    {
        case (NumberFormat::FormatType::Integer):
            result += QString::number(int(number));
            break;
        case (NumberFormat::FormatType::Decimal):
            result += QString::number(number, 'f', nDecimals_);
            break;
        case (NumberFormat::FormatType::Scientific):
            if (!useENotation_)
                result += QString::number(x.mantissa(), 'f', nDecimals_) + QChar(0x00D7) + "10\\sup{" +
                          QString::number(x.exponent()) + "}";
            else if (useUpperCaseExponent_)
                result += QString::number(x.mantissa(), 'f', nDecimals_) + "E" + QString::number(x.exponent());
            else
                result += QString::number(x.mantissa(), 'f', nDecimals_) + "e" + QString::number(x.exponent());
            break;
        default:
            result += QString::number(number);
            break;
    }

    return result;
}
