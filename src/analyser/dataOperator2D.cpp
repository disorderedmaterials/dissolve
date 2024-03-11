// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataOperator2D.h"
#include "math/data2D.h"
#include "math/integrator.h"

DataOperator2D::DataOperator2D(Data2D &targetData) : DataOperatorBase<Data2D, NormalisationFunction2D>(targetData) {}

void DataOperator2D::normalise(NormalisationFunction2D normalisationFunction)
{
    const auto &xs = targetData_.xAxis();
    const auto &ys = targetData_.yAxis();
    auto &values = targetData_.values();

    const auto xDelta = xs.size() > 1 ? xs[1] - xs[0] : 1.0;
    const auto yDelta = ys.size() > 1 ? ys[1] - ys[0] : 1.0;

    for (auto i = 0; i < xs.size(); ++i)
    {
        for (auto j = 0; j < ys.size(); ++j)
        {
            values[{i, j}] = normalisationFunction(xs[i], xDelta, ys[j], yDelta, values[{i, j}]);
        }
    }
}

void DataOperator2D::normaliseByGrid() { Messenger::warn("Grid normalisation not implemented for 2D data."); }

void DataOperator2D::normaliseBySphericalShell()
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData_.xAxis();
    const auto &yAxis = targetData_.yAxis();
    auto &values = targetData_.values();

    if (xAxis.size() < 2)
        return;

    // Derive first left-bin boundary from the delta between points 0 and 1
    auto leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5;
    auto r1Cubed = pow(leftBin, 3);
    for (auto n = 0; n < xAxis.size(); ++n)
    {
        auto r2Cubed = 0.0, rightBin = 0.0;
        for (auto m = 0; m < yAxis.size(); ++m)
        {
            // Get new right-bin from existing left bin boundary and current bin centre
            auto rightBin = leftBin + 2 * (xAxis[n] - leftBin);
            auto r2Cubed = pow(rightBin, 3);

            // Calculate divisor for normalisation
            auto divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);

            // Peform normalisation step
            values[{n, m}] /= divisor;
            if (targetData_.valuesHaveErrors())
                targetData_.error(n, m) /= divisor;
        }

        // Overwrite old values
        r1Cubed = r2Cubed;
        leftBin = rightBin;
    }
}

void DataOperator2D::normaliseTo(double value, bool absolute)
{
    auto sum = absolute ? Integrator::absSum(targetData_) : Integrator::sum(targetData_);
    targetData_ /= sum;
    targetData_ *= value;
}
