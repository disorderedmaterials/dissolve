// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/dataOperatorBase.h"
#include "math/data3D.h"

// x, xDelta, y, yDelta, z, zDelta, value
using OperateFunction3D = std::function<double(const double &, const double &, const double &, const double &, const double &,
                                               const double &, const double &)>;

// Data Operator 3D
class DataOperator3D : public DataOperatorBase<Data3D, OperateFunction3D>
{
    public:
    DataOperator3D(Data3D &targetData);

    /*
     * Data Operation Functions
     */
    public:
    // Generic operate function
    void operate(OperateFunction3D operateFunction) override;

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