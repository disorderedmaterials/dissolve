// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string_view>

// Forward declarations
class Configuration;

template <typename DataND> class DataNormaliserBase
{

    public:
    DataNormaliserBase(DataND &targetData) : targetData_(targetData) {}

    protected:
    DataND &targetData_;

    public:
    virtual void normaliseByNumberDensity(double population, Configuration *targetConfiguration) = 0;
    virtual void normaliseBySitePopulation(double population) = 0;
    virtual void normaliseBySphericalShell() = 0;
    virtual void normaliseByValue(double value = 1.0, bool absolute = true) = 0;
    virtual void normaliseByExpression(std::string_view expressionString) = 0;
    virtual void normaliseByGrid() = 0;
};