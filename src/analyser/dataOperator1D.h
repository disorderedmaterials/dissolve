// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data1D.h"

// x, xDelta, value
using OperateFunction1D = std::function<double(const double &, const double &, const double &)>;

// Data Operator 1D
class DataOperator1D : public DataOperatorBase<Data1D, OperateFunction1D>
{
    public:
    DataOperator1D(Data1D &targetData);

    /*
     * Data Operation Functions
     */
    public:
    // Generic operate function
    void operate(OperateFunction1D operateFunction) override;

    /*
     * Normalisation Functions
     */
    public:
    // Perform grid normalisation
    void normaliseByGrid() override;
    // Perform spherical shell normalisation
    void normaliseBySphericalShell() override;
    // Normalise the target data to a given value
    void normaliseSumTo(double value = 1.0, bool absolute = true) override;
};