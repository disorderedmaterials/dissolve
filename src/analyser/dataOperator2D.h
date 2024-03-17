// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data2D.h"

// x, xDelta, y, yDelta, value
using OperateFunction2D = std::function<double(const double &, const double &, const double &, const double &, const double &)>;

// Data Operator 2D
class DataOperator2D : public DataOperatorBase<Data2D, OperateFunction2D>
{
    public:
    DataOperator2D(Data2D &targetData);

    /*
     * Data Operation Functions
     */
    public:
    // Generic operate function
    void operate(OperateFunction2D operateFunction) override;

    /*
     * Normalisation Functions
     */
    public:
    // Perform grid normalisation
    void normaliseByGrid() override;
    // Perform spherical shell normalisation
    void normaliseBySphericalShell() override;
    // Normalise the target data to a given value
    void normaliseTo(double value = 1.0, bool absolute = true) override;
};