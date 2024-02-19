// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include <string_view>

template <typename DataND> class DataNormaliserBase
{

    public:
    DataNormaliserBase(DataND &targetData) : targetData_(targetData) {}

    protected:
    DataND &targetData_;

    public:
    void normaliseByNumberDensity(double population, Configuration *targetConfiguration)
    {
        targetData_ /= (population / targetConfiguration->box()->volume());
    }
    void normaliseBySitePopulation(double population) { targetData_ /= population; }
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseTo(double value = 1.0, bool absolute = true) = 0;
    virtual void normaliseByExpression(std::string_view expressionString) = 0;
    virtual void normaliseByGrid() = 0;
};