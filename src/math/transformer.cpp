// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/transformer.h"
#include "expression/variable.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"

Transformer::Transformer()
{
    // Create variables, and add them to the vector
    x_ = std::make_shared<ExpressionVariable>("x");
    variables_.emplace_back(x_);
    y_ = std::make_shared<ExpressionVariable>("y");
    variables_.emplace_back(y_);
    z_ = std::make_shared<ExpressionVariable>("z");
    variables_.emplace_back(z_);
    value_ = std::make_shared<ExpressionVariable>("value");
    variables_.emplace_back(value_);

    valid_ = false;
}

Transformer::~Transformer() = default;

Transformer::Transformer(const Transformer &source) { (*this) = source; }

void Transformer::operator=(const Transformer &source)
{
    // Set equation from old expression
    setEquation(source.equation_.expressionString());
    enabled_ = source.enabled_;
}

/*
 * Transform Equation
 */

// Set whether transform is enabled
void Transformer::setEnabled(bool b) { enabled_ = b; }

// Return whether transform is enabled
bool Transformer::enabled() const { return enabled_; }

// Set equation, returning if it was successfully generated
bool Transformer::setEquation(std::string_view equation)
{
    valid_ = equation_.create(equation, variables_);

    return valid_;
}

// Return text used to generate last equation
std::string_view Transformer::text() const { return equation_.expressionString(); }

// Return whether current equation is valid
bool Transformer::valid() const { return valid_; }

/*
 * Transform Values
 */

// Transform supplied Data1D values
void Transformer::transformValues(Data1D &data)
{
    // If Transformer isn't enabled, return now
    if (!enabled_)
        return;

    // Get references to x and value arrays, and take copies of each
    const auto &xAxis = data.xAxis();
    auto &values = data.values();

    // Data1D x and value (y) arrays are of same size - loop over number of values
    for (auto n = 0; n < data.nValues(); ++n)
    {
        // Set values in equations
        x_->setValue(xAxis[n]);
        y_->setValue(values[n]);
        value_->setValue(values[n]);

        // Perform transform
        values[n] = equation_.asDouble();
    }
}

// Transform supplied Data2D values
void Transformer::transformValues(Data2D &data)
{
    // If Transformer isn't enabled, return now
    if (!enabled_)
        return;

    // Get references to x and value arrays, and take copies of each
    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.yAxis();
    auto &values = data.values();

    // Data2D x and y arrays may be of different sizes
    for (auto i = 0; i < xAxis.size(); ++i)
    {
        // Set x value in equation
        x_->setValue(xAxis[i]);

        // Loop over Y axis points
        for (auto j = 0; j < yAxis.size(); ++j)
        {
            // Set y and value (z) values in equation
            y_->setValue(yAxis[j]);
            z_->setValue(values[{i, j}]);
            value_->setValue(values[{i, j}]);

            // Perform transform
            values[{i, j}] = equation_.asDouble();
        }
    }
}

// Transform supplied Data1D values
void Transformer::transformValues(Data3D &data)
{
    // If Transformer isn't enabled, return now
    if (!enabled_)
        return;

    // Get references to x and value arrays, and take copies of each
    const auto &xAxis = data.xAxis();
    const auto &yAxis = data.yAxis();
    const auto &zAxis = data.zAxis();
    auto &values = data.values();

    // Data3D x, y and z arrays may be of different sizes
    for (auto i = 0; i < xAxis.size(); ++i)
    {
        // Set x value in equation
        x_->setValue(xAxis[i]);

        // Loop over Y axis points
        for (auto j = 0; j < yAxis.size(); ++j)
        {
            // Set y and value (z) values in equation
            y_->setValue(yAxis[j]);

            // Loop over z values
            for (auto k = 0; k < zAxis.size(); ++k)
            {
                z_->setValue(values[std::tuple{i, j, k}]);
                value_->setValue(values[std::tuple{i, j, k}]);

                // Perform transform
                values[std::tuple{i, j, k}] = equation_.asDouble();
            }
        }
    }
}
