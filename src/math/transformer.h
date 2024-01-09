// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/expression.h"
#include "math/data3D.h"
#include "templates/array2D.h"

// Forward Declarations
class Data1D;
class Data2D;
class ExpressionVariable;

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
    // Vector of variables accessible by the transform equation
    std::vector<std::shared_ptr<ExpressionVariable>> variables_;
    // Variables accessible by the transform equation
    std::shared_ptr<ExpressionVariable> x_, y_, z_, value_;

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
