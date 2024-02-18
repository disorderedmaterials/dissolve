// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "expression/expression.h"
#include "expression/variable.h"
#include "math/data1D.h"
#include "math/integrator.h"

DataNormaliser1D::DataNormaliser1D(Data1D &targetData) : DataNormaliserBase<Data1D>(targetData) {}

void DataNormaliser1D::normaliseBySphericalShell()
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData_.xAxis();
    auto &values = targetData_.values();

    if (xAxis.size() < 2)
        return;

    // Derive first left-bin boundary from the delta between points 0 and 1
    auto leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5;
    auto r1Cubed = pow(leftBin, 3);
    for (auto n = 0; n < xAxis.size(); ++n)
    {
        // Get new right-bin from existing left bin boundary and current bin centre
        auto rightBin = leftBin + 2 * (xAxis[n] - leftBin);
        auto r2Cubed = pow(rightBin, 3);

        // Calculate divisor for normalisation
        auto divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);

        // Peform normalisation step
        values[n] /= divisor;
        if (targetData_.valuesHaveErrors())
            targetData_.error(n) /= divisor;

        // Overwrite old values for next iteration
        r1Cubed = r2Cubed;
        leftBin = rightBin;
    }
}

void DataNormaliser1D::normaliseByExpression(std::string_view expressionString)
{

    Expression expression;
    auto x = expression.addLocalVariable("x");
    auto xDelta = expression.addLocalVariable("xDelta");
    auto value = expression.addLocalVariable("value");

    expression.create(expressionString);

    const auto &xs = targetData_.xAxis();
    auto &values = targetData_.values();

    // Set data-related quantities
    if (xs.size() > 1)
        xDelta->setValue(xs[1] - xs[0]);

    // Evaluate the expression over all values
    for (auto i = 0; i < xs.size(); ++i)
    {
        // Set variables in expression
        x->setValue(xs[i]);
        value->setValue(values.at(i));

        // Evaluate and store new value
        values.at(i) = expression.asDouble();
    }
}

void DataNormaliser1D::normaliseByGrid() { Messenger::warn("Grid normalisation not implemented for 1D data."); }
