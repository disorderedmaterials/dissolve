// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include <string_view>

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
     * Generic Operate Functions
     */
    public:
    void divide(double divisor) { targetData_ /= divisor; }
    void multiply(double multiplier) { targetData_ *= multiplier; }
    virtual void operate(OperateFunction operateFunction) = 0;

    /*
     * Normalisation Functions
     */
    virtual void normaliseByGrid() = 0;
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseTo(double value = 1.0, bool absolute = true) = 0;
};