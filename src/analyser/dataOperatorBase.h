// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include <string_view>

// Data Operator Base
template <typename DataND, typename OperateFunction> class DataOperatorBase
{

    public:
    DataOperatorBase(DataND &targetData) : targetData_(targetData) {}

    /*
     * Targets
     */
    protected:
    DataND &targetData_;

    /*
     * Data Operation Functions
     */
    public:
    // Divide the target data by the divisor
    void divide(double divisor) { targetData_ /= divisor; }
    // Multiply the target data by the multiplier
    void multiply(double multiplier) { targetData_ *= multiplier; }
    // Generic operate function
    virtual void operate(OperateFunction operateFunction) = 0;

    /*
     * Normalisation Functions
     */
    public:
    // Perform grid normalisation
    virtual void normaliseByGrid() = 0;
    // Perform spherical shell normalisation
    virtual void normaliseBySphericalShell() = 0;
    // Normalise the target data to a given value
    virtual void normaliseTo(double value = 1.0, bool absolute = true) = 0;
};