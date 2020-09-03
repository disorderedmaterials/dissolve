/*
    *** Symmetry Generator
    *** src/data/sg/generator.h
    Copyright T. Youngs 2019-2020

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

#include "math/matrix4.h"

// Forward Declarations
/* none */

// Symmetry Generator
class SymmetryGenerator
{
    /*
     * Generator Information
     */
    private:
    // Generator text (if any)
    std::string text_;
    // Generator matrix
    Matrix4 matrix_;

    private:
    // Set partial element of matrix in specified row
    void setMatrixPart(int row, std::string_view part);

    public:
    // Set from plain text string
    bool set(std::string_view s);
    // Set rotation matrix row (not including translation vector)
    void setRotationRow(int row, double x, double y, double z);
    // Set translation column
    void setTranslation(double tx, double ty, double tz, double divisor);
    // Return generator text
    std::string_view text() const;
    // Return matrix for generator
    const Matrix4 &matrix() const;
};
