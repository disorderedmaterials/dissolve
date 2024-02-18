// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser2D.h"
#include "classes/configuration.h"
#include "expression/expression.h"
#include "expression/variable.h"
#include "math/data2D.h"
#include "math/integrator.h"

DataNormaliser2D::DataNormaliser2D(Data2D &targetData) : DataNormaliserBase(targetData) {}

void DataNormaliser2D::normaliseBySphericalShell()
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData_.xAxis();
    const auto &yAxis = targetData_.yAxis();
    auto &values = targetData_.values();
    if (xAxis.size() > 2)
    {
        // Derive first left-bin boundary from the delta between points 0 and 1
        double leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5, rightBin, divisor;
        double r1Cubed = pow(leftBin, 3), r2Cubed;
        for (auto n = 0; n < xAxis.size(); ++n)
        {
            for (auto m = 0; m < yAxis.size(); ++m)
            {
                // Get new right-bin from existing left bin boundary and current bin centre
                rightBin = leftBin + 2 * (xAxis[n] - leftBin);
                r2Cubed = pow(rightBin, 3);
                divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);
                values[{n, m}] /= divisor;
                if (targetData_.valuesHaveErrors())
                    targetData_.error(n, m) /= divisor;
            }

            // Overwrite old values
            r1Cubed = r2Cubed;
            leftBin = rightBin;
        }
    }
}

void DataNormaliser2D::normaliseByExpression(std::string_view expressionString)
{
    Expression expression;
    auto x = expression.addLocalVariable("x");
    auto xDelta = expression.addLocalVariable("xDelta");
    auto y = expression.addLocalVariable("y");
    auto yDelta = expression.addLocalVariable("yDelta");
    auto value = expression.addLocalVariable("value");

    const auto &xs = targetData_.xAxis();
    const auto &ys = targetData_.yAxis();
    auto &values = targetData_.values();

    // Set data-related quantities
    if (xs.size() > 1)
        xDelta->setValue(xs[1] - xs[0]);
    if (ys.size() > 1)
        yDelta->setValue(ys[1] - ys[0]);

    expression.create(expressionString);

    // Evaluate the expression over all values
    for (auto i = 0; i < xs.size(); ++i)
    {
        // Set x value in expression
        x->setValue(xs[i]);

        for (auto j = 0; j < ys.size(); ++j)
        {
            // Set y and value in expression
            y->setValue(ys[j]);
            value->setValue(values[{i, j}]);

            // Evaluate and store new value
            values[{i, j}] = expression.asDouble();
        }
    }
}

void DataNormaliser2D::normaliseByGrid() { ; }