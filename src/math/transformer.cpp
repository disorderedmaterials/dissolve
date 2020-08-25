/*
    *** Transformer
    *** src/math/transformer.cpp
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

#include "math/transformer.h"
#include "data3d.h"
#include "expression/generator.h"
#include "expression/variable.h"
#include "math/data1d.h"
#include "math/data2d.h"
#include "templates/array.h"

Transformer::Transformer()
{
    // Add persistent variable trio to equation
    x_ = equation_.createDoubleVariable("x", true);
    y_ = equation_.createDoubleVariable("y", true);
    z_ = equation_.createDoubleVariable("z", true);
    value_ = equation_.createDoubleVariable("value", true);
    valid_ = false;
}

Transformer::~Transformer() {}

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
    valid_ = equation_.set(equation);

    return valid_;
}

// Return text used to generate last equation_
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
    const auto &xAxis = data.constXAxis();
    Array<double> &values = data.values();

    // Data1D x and value (y) arrays are of same size - loop over number of values
    for (int n = 0; n < data.nValues(); ++n)
    {
        // Set values in equations
        x_->set(xAxis.constAt(n));
        y_->set(values.constAt(n));
        value_->set(values.constAt(n));

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
    const auto &xAxis = data.constXAxis();
    const auto &yAxis = data.constYAxis();
    Array2D<double> &values = data.values();

    // Data2D x and y arrays may be of different sizes
    for (int i = 0; i < xAxis.nItems(); ++i)
    {
        // Set x value in equation
        x_->set(xAxis.constAt(i));

        // Loop over Y axis points
        for (int j = 0; j < yAxis.nItems(); ++j)
        {
            // Set y and value (z) values in equation
            y_->set(yAxis.constAt(j));
            z_->set(values.at(i, j));
            value_->set(values.at(i, j));

            // Perform transform
            values.at(i, j) = equation_.asDouble();
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
    const auto &xAxis = data.constXAxis();
    const auto &yAxis = data.constYAxis();
    const auto &zAxis = data.constZAxis();
    Array3D<double> &values = data.values();

    // Data3D x, y and z arrays may be of different sizes
    for (int i = 0; i < xAxis.nItems(); ++i)
    {
        // Set x value in equation
        x_->set(xAxis.constAt(i));

        // Loop over Y axis points
        for (int j = 0; j < yAxis.nItems(); ++j)
        {
            // Set y and value (z) values in equation
            y_->set(yAxis.constAt(j));

            // Loop over z values
            for (int k = 0; k < zAxis.nItems(); ++k)
            {
                z_->set(values.at(i, j, k));
                value_->set(values.at(i, j, k));

                // Perform transform
                values.at(i, j, k) = equation_.asDouble();
            }
        }
    }
}
