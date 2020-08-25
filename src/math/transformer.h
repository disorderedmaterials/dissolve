/*
    *** Transformer
    *** src/math/transformer.h
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

#include "data3d.h"
#include "expression/expression.h"
#include "templates/array.h"
#include "templates/array2d.h"

// Forward Declarations
class Data1D;
class Data2D;
class Variable;

// Transformer
class Transformer
{
    public:
    Transformer();
    ~Transformer();
    Transformer(const Transformer &source);
    void operator=(const Transformer &source);

    /*
     * Transform Equation
     */
    private:
    // Whether transform is enabled
    bool enabled_;
    // Transform equation
    Expression equation_;
    // Whether current equation is valid
    bool valid_;
    // X variable in equation
    ExpressionVariable *x_;
    // Y variable in equation
    ExpressionVariable *y_;
    // Z variable in equation
    ExpressionVariable *z_;
    // Value variable in equation
    ExpressionVariable *value_;

    public:
    // Set whether transform is enabled
    void setEnabled(bool b);
    // Return whether transform is enabled
    bool enabled() const;
    // Set equation, returning if it was successfully generated
    bool setEquation(std::string_view equation);
    // Return text used to generate last equation
    std::string_view text() const;
    // Return whether current equation is valid
    bool valid() const;

    /*
     * Transforms
     */
    public:
    // Transform supplied Data1D values
    void transformValues(Data1D &data);
    // Transform supplied Data2D values
    void transformValues(Data2D &data);
    // Transform supplied Data3D values
    void transformValues(Data3D &data);
};
