// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataNormaliser1D.h"
#include "classes/configuration.h"
#include "math/data1D.h"

DataNormaliser1D::DataNormaliser1D(Data1D *targetData) : targetData_(targetData) {}

void DataNormaliser1D::normaliseByNumberDensity(double population, Configuration *targetConfiguration)
{
    (*targetData_) /= (population / targetConfiguration->box()->volume());
}

void DataNormaliser1D::normaliseBySitePopulation(double population) { (*targetData_) /= population; }

void DataNormaliser1D::normaliseBySphericalShell()
{
    // We expect x values to be centre-bin values, and regularly spaced
    const auto &xAxis = targetData_->xAxis();
    auto &values = targetData_->values();

    if (xAxis.size() < 2)
        return;

    // Derive first left-bin boundary from the delta between points 0 and 1
    double leftBin = xAxis[0] - (xAxis[1] - xAxis[0]) * 0.5, rightBin, divisor;
    double r1Cubed = pow(leftBin, 3), r2Cubed;
    for (auto n = 0; n < xAxis.size(); ++n)
    {
        // Get new right-bin from existing left bin boundary and current bin centre
        rightBin = leftBin + 2 * (xAxis[n] - leftBin);
        r2Cubed = pow(rightBin, 3);

        // Calculate divisor for normalisation
        divisor = (4.0 / 3.0) * PI * (r2Cubed - r1Cubed);

        // Peform normalisation step
        values[n] /= divisor;
        if (targetData_->valuesHaveErrors())
            targetData_->error(n) /= divisor;

        // Overwrite old values for next iteration
        r1Cubed = r2Cubed;
        leftBin = rightBin;
    }
}