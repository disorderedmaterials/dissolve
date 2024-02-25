// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser2D.h"
#include "math/data2D.h"
#include "math/integrator.h"

DataNormaliser2D::DataNormaliser2D(Data2D &targetData) : DataNormaliserBase<Data2D, NormalisationFunction2D>(targetData) {}

void DataNormaliser2D::normalise(NormalisationFunction2D normalisationFunction)
{
    const auto &xs = targetData_.xAxis();
    const auto &ys = targetData_.yAxis();
    auto &values = targetData_.values();

    for (auto i = 0; i < xs.size(); ++i)
    {
        for (auto j = 0; j < ys.size(); ++j)
        {
            values[{i, j}] = normalisationFunction(xs[i], ys[j], values[{i, j}]);
        }
    }
}

void DataNormaliser2D::normaliseByGrid() { Messenger::warn("Grid normalisation not implemented for 2D data."); }

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

void DataNormaliser2D::normaliseTo(double value, bool absolute)
{
    auto sum = absolute ? Integrator::absSum(targetData_) : Integrator::sum(targetData_);
    targetData_ /= sum;
    targetData_ *= value;
}
