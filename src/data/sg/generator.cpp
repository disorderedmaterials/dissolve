// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/sg/generator.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "data/sg/sginfo.h"
#include <string.h>

// Set partial element of matrix or translation vector
void SymmetryGenerator::setMatrixPart(int row, std::string_view part)
{
    // The string provided either contains (-)xyz, or a translation amount
    auto pos = 0;
    auto multiplier = 0;
    std::string a, b;

    // Check for plus/minus signs
    if (part[0] == '-')
        multiplier = -1;
    else if (part[0] == '+')
        multiplier = 1;

    // Skip to next character if necessary
    if (multiplier == 0)
        multiplier = 1;
    else
        ++pos;

    // Now, check if this character is x, y, or z.
    switch (part[pos])
    {
        case ('x'):
        case ('X'):
            matrix_[row] = multiplier;
            break;
        case ('y'):
        case ('Y'):
            matrix_[4 + row] = multiplier;
            break;
        case ('z'):
        case ('Z'):
            matrix_[8 + row] = multiplier;
            break;
        default:
            // Must be a number, and hence part of the translation vector
            a = DissolveSys::beforeChar(part, '/');
            b = DissolveSys::afterChar(part, '/');
            matrix_[12 + row] = std::stof(a) / std::stof(b);
            break;
    }
}

// Set from plain text string
bool SymmetryGenerator::set(std::string_view s)
{
    LineParser parser;

    // Split line into three arguments
    parser.getArgsDelim(LineParser::Defaults, s);
    if (parser.nArgs() != 3)
    {
        Messenger::print("Tried to set a symmetry Generator from text ('{}') that didn't split into three arguments.", s);
        return false;
    }

    // Clear any existing data
    matrix_.zero();

    // Loop over arguments (equivalent to rows in matrix) and set parameters
    for (auto n = 0; n < 3; ++n)
    {
        // Step through characters in 'part', adding until we find a (second) 'delimiting' character
        std::string subString;
        for (int i = 0; i < parser.argsv(n).length(); ++i)
        {
            char c = parser.argsv(n)[i];
            // Store old sub-part if we find a new delimiting character, and our subString is not empty
            if (((c == '-') || (c == '+')) && (!subString.empty()))
            {
                // This constitutes a sub-part of the string
                setMatrixPart(n, subString);
                subString.clear();
            }
            subString += c;
        }

        // Check for remaining 'item'
        if (!subString.empty())
            setMatrixPart(n, subString);
    }

    return true;
}

// Set rotation matrix row (not including translation vector)
void SymmetryGenerator::setRotationRow(int row, double x, double y, double z) { matrix_.setRow(row, x, y, z); }

// Set translation column
void SymmetryGenerator::setTranslation(double tx, double ty, double tz, double divisor)
{
    matrix_[12] = tx / divisor;
    matrix_[13] = ty / divisor;
    matrix_[14] = tz / divisor;
}

// Return generator text
std::string_view SymmetryGenerator::text() const { return text_; }

// Return matrix of generator
const Matrix4 &SymmetryGenerator::matrix() const { return matrix_; }
