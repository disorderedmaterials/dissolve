// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataOperator1D.h"
#include "math/data1D.h"
#include "math/integrator.h"

DataOperator1D::DataOperator1D(Data1D &targetData) : DataOperatorBase<Data1D, OperateFunction1D>(targetData) {}

void DataOperator1D::operate(OperateFunction1D operateFunction)
{
    const auto &xs = targetData_.xAxis();
    auto &values = targetData_.values();

    for (auto i = 0; i < xs.size(); ++i)
        values.at(i) = operateFunction(xs[i], targetData_.error(i), values.at(i));
}

void DataOperator1D::normaliseByGrid() { Messenger::warn("Grid normalisation not implemented for 1D data."); }

void DataOperator1D::normaliseBySphericalShell()
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

void DataOperator1D::normaliseTo(double value, bool absolute)
{
    auto sum = absolute ? Integrator::absSum(targetData_) : Integrator::sum(targetData_);
    targetData_ /= sum;
    targetData_ *= value;
}
