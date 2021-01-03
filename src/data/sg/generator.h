// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
