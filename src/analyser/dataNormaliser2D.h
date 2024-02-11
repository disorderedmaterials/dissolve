// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"
#include <string_view>

class Configuration;
class Data2D;

class DataNormaliser2D
{
    public:
    DataNormaliser1D(Data2D &targetData);

    /*
     * Target
     */
    private:
    // Target data to normalise
    Data1D &targetData_;

    /*
     * Normalisation functions
     */
    public:
    void normaliseByNumberDensity(double population, Configuration *targetConfiguration);
    void normaliseBySitePopulation(double population);
    void normaliseBySphericalShell();
    void normaliseByValue(double value = 1.0, bool absolute = true);
    void normaliseByExpression(std::string_view expressionString);
};