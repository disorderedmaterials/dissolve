// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include <string_view>

template <typename DataND, typename NormalisationFunction> class DataOperatorBase
{

    public:
    DataOperatorBase(DataND &targetData) : targetData_(targetData) {}

    /*
     * Targets
     */
    protected:
    DataND &targetData_;

    /*
     * Normalisation functions
     */
    public:
    void normaliseDivide(double divisor) { targetData_ /= divisor; }
    virtual void normalise(NormalisationFunction normalisationFunction) = 0;
    virtual void normaliseByGrid() = 0;
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseTo(double value = 1.0, bool absolute = true) = 0;
};